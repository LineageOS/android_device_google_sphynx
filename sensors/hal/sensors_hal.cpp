/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <hardware/hardware.h>
#include <hardware/sensors.h>
#include <log/log.h>

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "pixelc_sensors/iio_engine.h"

namespace {

// SensorService requires a positive handle. The EC accelerometer ID is zero.
constexpr int kAccelerometerHandle = 1;

const sensor_t kAccelerometer = {
        .name = "CrosEC Accelerometer",
        .vendor = "Google",
        .version = 1,
        .handle = kAccelerometerHandle,
        .type = SENSOR_TYPE_ACCELEROMETER,
        .maxRange = pixelc::sensors::kAccelerometerMaxRange,
        .resolution = pixelc::sensors::kAccelerometerScale,
        .power = 0.18f,
        .minDelay = 5000,
        .fifoReservedEventCount = 0,
        .fifoMaxEventCount = 2048 * 2 / 3,
        .stringType = SENSOR_STRING_TYPE_ACCELEROMETER,
        .requiredPermission = nullptr,
        .maxDelay = 80000,
        .flags = SENSOR_FLAG_CONTINUOUS_MODE,
        .reserved = {nullptr, nullptr},
};

class PixelCSensorsDevice {
  public:
    explicit PixelCSensorsDevice(const hw_module_t* module) {
        std::memset(&device_, 0, sizeof(device_));
        device_.common.tag = HARDWARE_DEVICE_TAG;
        device_.common.version = SENSORS_DEVICE_API_VERSION_1_3;
        device_.common.module = const_cast<hw_module_t*>(module);
        device_.common.close = Close;
        device_.activate = Activate;
        device_.setDelay = SetDelay;
        device_.poll = Poll;
        device_.batch = Batch;
        device_.flush = Flush;
        if (pipe2(wake_fds_, O_CLOEXEC | O_NONBLOCK) != 0) {
            init_error_ = -errno;
            wake_fds_[0] = wake_fds_[1] = -1;
        }
    }

    ~PixelCSensorsDevice() {
        engine_.Restore();
        if (wake_fds_[0] >= 0) close(wake_fds_[0]);
        if (wake_fds_[1] >= 0) close(wake_fds_[1]);
    }

    sensors_poll_device_1_t* device() { return &device_; }

    int Init() {
        if (init_error_ != 0) {
            ALOGE("Pixel C sensors: cannot create wake pipe: %s", std::strerror(-init_error_));
            return init_error_;
        }
        std::string error;
        if (!engine_.Discover(&error)) {
            ALOGE("Pixel C sensors: IIO discovery failed: %s", error.c_str());
            return -ENODEV;
        }
        const auto& topology = engine_.topology();
        ALOGI("Pixel C sensors: IIO provider=%s ring=%s trigger=%s",
              topology.provider_name.c_str(), topology.ring_name.c_str(),
              topology.trigger_name.c_str());
        return 0;
    }

  private:
    static PixelCSensorsDevice* From(sensors_poll_device_t* device) {
        return reinterpret_cast<PixelCSensorsDevice*>(device);
    }

    static PixelCSensorsDevice* From(sensors_poll_device_1_t* device) {
        return reinterpret_cast<PixelCSensorsDevice*>(device);
    }

    static int Close(hw_device_t* device) {
        delete reinterpret_cast<PixelCSensorsDevice*>(device);
        return 0;
    }

    static int Activate(sensors_poll_device_t* device, int handle, int enabled) {
        if (handle != kAccelerometerHandle) return -EINVAL;
        PixelCSensorsDevice* self = From(device);
        std::string error;
        if (!self->engine_.Activate(enabled != 0, &error)) return -EIO;
        self->enabled_ = enabled != 0;
        self->Wake();
        return 0;
    }

    static int SetDelay(sensors_poll_device_t* device, int handle, int64_t ns) {
        return Batch(reinterpret_cast<sensors_poll_device_1_t*>(device),
                     handle, 0, ns, 0);
    }

    static int Batch(sensors_poll_device_1_t* device, int handle, int flags,
                     int64_t period_ns, int64_t latency_ns) {
        if (handle != kAccelerometerHandle || period_ns <= 0 || latency_ns < 0) {
            return -EINVAL;
        }
        if ((flags & ~SENSORS_BATCH_DRY_RUN) != 0) return -EINVAL;
        if ((flags & SENSORS_BATCH_DRY_RUN) != 0) return 0;
        PixelCSensorsDevice* self = From(device);
        std::string error;
        return self->engine_.SetSampling(period_ns, latency_ns, &error) ? 0 : -EIO;
    }

    static int Flush(sensors_poll_device_1_t* device, int handle) {
        if (handle != kAccelerometerHandle) return -EINVAL;
        PixelCSensorsDevice* self = From(device);
        if (!self->enabled_) return -EINVAL;
        self->flush_pending_.fetch_add(1);
        self->Wake();
        return 0;
    }

    static int Poll(sensors_poll_device_t* device, sensors_event_t* output,
                    int count) {
        if (output == nullptr || count <= 0) return -EINVAL;
        PixelCSensorsDevice* self = From(device);
        for (;;) {
            pollfd fds[2] = {
                    {self->engine_.fd(), POLLIN, 0},
                    {self->wake_fds_[0], POLLIN, 0},
            };
            int result;
            do {
                result = poll(fds, 2, -1);
            } while (result < 0 && errno == EINTR);
            if (result < 0) return -errno;

            int written = 0;
            if (fds[0].revents & POLLIN) {
                std::vector<pixelc::sensors::AndroidAccelEvent> events;
                std::string error;
                const int read = self->engine_.Read(&events, count, &error);
                if (read < 0) return read;
                for (const auto& event : events) {
                    sensors_event_t& out = output[written++];
                    std::memset(&out, 0, sizeof(out));
                    out.version = sizeof(out);
                    out.sensor = kAccelerometerHandle;
                    out.type = SENSOR_TYPE_ACCELEROMETER;
                    out.timestamp = event.timestamp_ns;
                    out.acceleration.x = event.x;
                    out.acceleration.y = event.y;
                    out.acceleration.z = event.z;
                    out.acceleration.status = SENSOR_STATUS_ACCURACY_LOW;
                }
            }
            if (fds[1].revents & POLLIN) {
                char discard[16];
                while (read(self->wake_fds_[0], discard, sizeof(discard)) > 0) {}
            }
            if (written < count && self->flush_pending_.load() > 0) {
                self->flush_pending_.fetch_sub(1);
                sensors_event_t& out = output[written++];
                std::memset(&out, 0, sizeof(out));
                out.version = META_DATA_VERSION;
                out.type = SENSOR_TYPE_META_DATA;
                out.meta_data.what = META_DATA_FLUSH_COMPLETE;
                out.meta_data.sensor = kAccelerometerHandle;
            }
            if (written > 0) return written;
        }
    }

    void Wake() {
        if (wake_fds_[1] >= 0) {
            const char byte = 'W';
            (void)write(wake_fds_[1], &byte, 1);
        }
    }

    // This must remain first: the hw_device_t pointer is cast back to the object.
    sensors_poll_device_1_t device_{};
    pixelc::sensors::IioEngine engine_{};
    int wake_fds_[2] = {-1, -1};
    int init_error_ = 0;
    bool enabled_ = false;
    std::atomic<int> flush_pending_{0};
};

int GetSensorsList(sensors_module_t*, const sensor_t** list) {
    *list = &kAccelerometer;
    return 1;
}

int OpenSensors(const hw_module_t* module, const char*, hw_device_t** device) {
    std::unique_ptr<PixelCSensorsDevice> instance(new PixelCSensorsDevice(module));
    const int result = instance->Init();
    if (result != 0) return result;
    *device = &instance->device()->common;
    instance.release();
    return 0;
}

hw_module_methods_t kModuleMethods = {
        .open = OpenSensors,
};

}  // namespace

extern "C" {
sensors_module_t HAL_MODULE_INFO_SYM = {
        .common = {
                .tag = HARDWARE_MODULE_TAG,
                .module_api_version = SENSORS_MODULE_API_VERSION_0_1,
                .hal_api_version = HARDWARE_HAL_API_VERSION,
                .id = SENSORS_HARDWARE_MODULE_ID,
                .name = "Pixel C Cros-EC sensor hub module",
                .author = "Google / Pixel C restoration project",
                .methods = &kModuleMethods,
                .dso = nullptr,
                .reserved = {},
        },
        .get_sensors_list = GetSensorsList,
        .set_operation_mode = nullptr,
};
}
