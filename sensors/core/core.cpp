/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pixelc_sensors/core.h"

#include <algorithm>
#include <cstring>

namespace pixelc::sensors {
namespace {

std::uint16_t ReadLe16(const std::uint8_t* data) {
    return static_cast<std::uint16_t>(data[0]) |
           (static_cast<std::uint16_t>(data[1]) << 8);
}

std::uint64_t ReadLe64(const std::uint8_t* data) {
    std::uint64_t value = 0;
    for (int i = 0; i < 8; ++i) {
        value |= static_cast<std::uint64_t>(data[i]) << (i * 8);
    }
    return value;
}

}  // namespace

bool DecodeRingEvent(const std::uint8_t* bytes, std::size_t size,
                     RingEvent* event) {
    if (bytes == nullptr || event == nullptr || size < kRingEventSize) {
        return false;
    }
    event->sensor_id = bytes[0];
    event->flags = bytes[1];
    event->x = static_cast<std::int16_t>(ReadLe16(bytes + 2));
    event->y = static_cast<std::int16_t>(ReadLe16(bytes + 4));
    event->z = static_cast<std::int16_t>(ReadLe16(bytes + 6));
    event->timestamp_ns = static_cast<std::int64_t>(ReadLe64(bytes + 8));
    return true;
}

AndroidAccelEvent ConvertAccelerometer(const RingEvent& event) {
    // Ryu EC firmware already rotates BMI160 samples through
    // accelgyro_standard_ref. Google's stock HAL applied no further matrix.
    return {
            static_cast<float>(event.x) * kAccelerometerScale,
            static_cast<float>(event.y) * kAccelerometerScale,
            static_cast<float>(event.z) * kAccelerometerScale,
            event.timestamp_ns,
    };
}

SamplingConfig MakeSamplingConfig(std::int64_t requested_period_ns,
                                  std::int64_t requested_latency_ns) {
    const std::int64_t period = std::clamp(
            requested_period_ns, kMinSamplingPeriodNs, kMaxSamplingPeriodNs);
    const std::int64_t latency = std::max(requested_latency_ns, period);
    return {
            period,
            latency,
            static_cast<std::int32_t>(1'000'000'000'000LL / period),
            static_cast<std::int32_t>(
                    std::min<std::int64_t>(kMaxEcRateMs,
                                           latency / 1'000'000LL)),
    };
}

}  // namespace pixelc::sensors
