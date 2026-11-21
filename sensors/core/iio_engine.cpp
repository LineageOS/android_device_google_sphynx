#include "pixelc_sensors/iio_engine.h"

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <utility>

namespace pixelc::sensors {
namespace {

bool StartsWith(const std::string& value, const char* prefix) {
    return value.compare(0, std::strlen(prefix), prefix) == 0;
}

bool EndsWith(const std::string& value, const char* suffix) {
    const std::size_t length = std::strlen(suffix);
    return value.size() >= length &&
           value.compare(value.size() - length, length, suffix) == 0;
}

std::string ReadText(const std::string& path) {
    std::ifstream input(path);
    std::string value;
    std::getline(input, value);
    return value;
}

bool Exists(const std::string& path) {
    return access(path.c_str(), F_OK) == 0;
}

std::vector<std::string> List(const std::string& path) {
    std::vector<std::string> entries;
    DIR* directory = opendir(path.c_str());
    if (directory == nullptr) return entries;
    while (dirent* entry = readdir(directory)) {
        if (entry->d_name[0] != '.') entries.emplace_back(entry->d_name);
    }
    closedir(directory);
    std::sort(entries.begin(), entries.end());
    return entries;
}

}  // namespace

IioEngine::IioEngine(std::string sysfs_root, std::string dev_root)
    : sysfs_root_(std::move(sysfs_root)),
      dev_root_(std::move(dev_root)),
      sampling_(MakeSamplingConfig(kMaxSamplingPeriodNs, 0)) {}

IioEngine::~IioEngine() {
    Restore();
}

bool IioEngine::Discover(std::string* error) {
    topology_ = {};
    for (const std::string& entry : List(sysfs_root_)) {
        const std::string path = sysfs_root_ + "/" + entry;
        const std::string name = ReadText(path + "/name");
        if (StartsWith(entry, "iio:device")) {
            if (name == "cros-ec-ring") {
                topology_.ring_name = entry;
                topology_.ring_path = path;
                topology_.ring_device_path = dev_root_ + "/" + entry;
            } else if (StartsWith(name, "cros-ec-sensors.") &&
                       Exists(path + "/in_accel_x_raw")) {
                topology_.provider_name = entry;
                topology_.provider_path = path;
            }
        } else if (StartsWith(entry, "trigger") &&
                   StartsWith(name, "cros-ec-ring-trigger")) {
            topology_.trigger_name = name;
        }
    }
    if (topology_.provider_path.empty() || topology_.ring_path.empty() ||
        topology_.trigger_name.empty()) {
        if (error) {
            *error = "incomplete Cros-EC IIO topology: provider='" +
                     topology_.provider_path + "' ring='" +
                     topology_.ring_path + "' trigger='" +
                     topology_.trigger_name + "'";
        }
        return false;
    }
    return true;
}

bool IioEngine::Write(const std::string& path, const std::string& value,
                      std::string* error) {
    const int fd = open(path.c_str(), O_WRONLY | O_CLOEXEC);
    if (fd < 0) {
        if (error) *error = "open " + path + ": " + std::strerror(errno);
        return false;
    }
    const ssize_t result = write(fd, value.data(), value.size());
    const int saved_errno = errno;
    close(fd);
    if (result != static_cast<ssize_t>(value.size())) {
        if (error) *error = "write " + path + ": " + std::strerror(saved_errno);
        return false;
    }
    return true;
}

bool IioEngine::ConfigureChannels(bool enabled, std::string* error) {
    const std::string scan = topology_.ring_path + "/scan_elements";
    std::vector<std::pair<int, std::string>> channels;
    for (const std::string& entry : List(scan)) {
        if (!EndsWith(entry, "_en")) continue;
        const std::string stem = entry.substr(0, entry.size() - 3);
        const std::string index = ReadText(scan + "/" + stem + "_index");
        if (index.empty()) continue;
        char* end = nullptr;
        const long parsed = std::strtol(index.c_str(), &end, 10);
        if (end == index.c_str() || *end != '\0') continue;
        channels.emplace_back(static_cast<int>(parsed), scan + "/" + entry);
    }
    std::sort(channels.begin(), channels.end());
    if (channels.size() != 6) {
        if (error) *error = "expected six Cros-EC scan channels, found " +
                            std::to_string(channels.size());
        return false;
    }
    for (const auto& channel : channels) {
        if (!Write(channel.second, enabled ? "1" : "0", error)) return false;
    }
    return true;
}

bool IioEngine::Prepare(std::string* error) {
    if (topology_.ring_path.empty() && !Discover(error)) return false;
    saved_frequency_ = ReadText(topology_.provider_path + "/frequency");
    saved_sampling_frequency_ =
            ReadText(topology_.provider_path + "/sampling_frequency");
    saved_buffer_enable_ = ReadText(topology_.ring_path + "/buffer/enable");
    saved_buffer_length_ = ReadText(topology_.ring_path + "/buffer/length");
    saved_trigger_ =
            ReadText(topology_.ring_path + "/trigger/current_trigger");
    saved_channel_states_.clear();
    const std::string scan = topology_.ring_path + "/scan_elements";
    for (const std::string& entry : List(scan)) {
        if (EndsWith(entry, "_en")) {
            saved_channel_states_.emplace_back(scan + "/" + entry,
                                               ReadText(scan + "/" + entry));
        }
    }
    saved_state_valid_ = true;
    if (!Write(topology_.ring_path + "/buffer/enable", "0", error) ||
        !ConfigureChannels(true, error) ||
        !Write(topology_.ring_path + "/trigger/current_trigger",
               topology_.trigger_name, error) ||
        !Write(topology_.ring_path + "/buffer/length", "4096", error)) {
        Restore();
        return false;
    }
    ring_fd_ = open(topology_.ring_device_path.c_str(), O_RDONLY | O_CLOEXEC);
    if (ring_fd_ < 0) {
        if (error) *error = "open " + topology_.ring_device_path + ": " +
                            std::strerror(errno);
        Restore();
        return false;
    }
    if (!Write(topology_.ring_path + "/buffer/enable", "1", error)) {
        Restore();
        return false;
    }
    prepared_ = true;
    return true;
}

bool IioEngine::SetSampling(std::int64_t period_ns, std::int64_t latency_ns,
                            std::string* error) {
    sampling_ = MakeSamplingConfig(period_ns, latency_ns);
    if (!enabled_) return true;
    return Write(topology_.provider_path + "/frequency",
                 std::to_string(sampling_.odr_millihz), error) &&
           Write(topology_.provider_path + "/sampling_frequency",
                 std::to_string(sampling_.ec_rate_ms), error);
}

bool IioEngine::Activate(bool enabled, std::string* error) {
    if (enabled && !prepared_ && !Prepare(error)) return false;
    if (enabled) {
        if (!Write(topology_.provider_path + "/frequency",
                   std::to_string(sampling_.odr_millihz), error) ||
            !Write(topology_.provider_path + "/sampling_frequency",
                   std::to_string(sampling_.ec_rate_ms), error)) {
            Restore();
            return false;
        }
        enabled_ = true;
        return true;
    }
    bool ok = true;
    if (!topology_.provider_path.empty()) {
        ok = Write(topology_.provider_path + "/frequency", "0", error) && ok;
        ok = Write(topology_.provider_path + "/sampling_frequency", "0", error) && ok;
    }
    enabled_ = false;
    return ok;
}

int IioEngine::Read(std::vector<AndroidAccelEvent>* events,
                    std::size_t max_events, std::string* error) {
    if (events == nullptr || ring_fd_ < 0 || max_events == 0) return -EINVAL;
    std::vector<std::uint8_t> buffer(max_events * kRingEventSize);
    const ssize_t count = read(ring_fd_, buffer.data(), buffer.size());
    if (count < 0) {
        if (error) *error = "read " + topology_.ring_device_path + ": " +
                            std::strerror(errno);
        return -errno;
    }
    if ((count % static_cast<ssize_t>(kRingEventSize)) != 0) {
        if (error) *error = "partial Cros-EC FIFO event";
        return -EINVAL;
    }
    for (ssize_t offset = 0; offset < count; offset += kRingEventSize) {
        RingEvent event{};
        DecodeRingEvent(buffer.data() + offset, kRingEventSize, &event);
        if (event.sensor_id == kAccelerometerSensorId &&
            (event.flags & kFlushFlag) == 0) {
            events->push_back(ConvertAccelerometer(event));
        }
    }
    return static_cast<int>(events->size());
}

void IioEngine::Restore() {
    std::string ignored;
    if (!topology_.ring_path.empty()) {
        Write(topology_.ring_path + "/buffer/enable", "0", &ignored);
    }
    if (ring_fd_ >= 0) close(ring_fd_);
    if (saved_state_valid_) {
        for (const auto& channel : saved_channel_states_) {
            Write(channel.first, channel.second, &ignored);
        }
        Write(topology_.ring_path + "/trigger/current_trigger",
              saved_trigger_.empty() ? "\n" : saved_trigger_, &ignored);
        if (!saved_buffer_length_.empty()) {
            Write(topology_.ring_path + "/buffer/length", saved_buffer_length_,
                  &ignored);
        }
        if (!saved_frequency_.empty()) {
            Write(topology_.provider_path + "/frequency", saved_frequency_,
                  &ignored);
        }
        if (!saved_sampling_frequency_.empty()) {
            Write(topology_.provider_path + "/sampling_frequency",
                  saved_sampling_frequency_, &ignored);
        }
        if (!saved_buffer_enable_.empty()) {
            Write(topology_.ring_path + "/buffer/enable", saved_buffer_enable_,
                  &ignored);
        }
    } else if (!topology_.provider_path.empty()) {
        Write(topology_.provider_path + "/frequency", "0", &ignored);
        Write(topology_.provider_path + "/sampling_frequency", "0", &ignored);
    }
    ring_fd_ = -1;
    prepared_ = false;
    enabled_ = false;
    saved_state_valid_ = false;
    saved_channel_states_.clear();
}

}  // namespace pixelc::sensors
