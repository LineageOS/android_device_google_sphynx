/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace pixelc::sensors {

constexpr std::size_t kRingEventSize = 16;
constexpr std::uint8_t kAccelerometerSensorId = 0;
constexpr std::uint8_t kFlushFlag = 0x01;
constexpr std::int64_t kMinSamplingPeriodNs = 5'000'000;
constexpr std::int64_t kMaxSamplingPeriodNs = 80'000'000;
constexpr std::int64_t kMaxEcRateMs = 65'534;
constexpr float kGravityEarth = 9.80665f;
constexpr float kAccelerometerRangeG = 8.0f;
constexpr float kAccelerometerScale =
        kAccelerometerRangeG * kGravityEarth / 32768.0f;
constexpr float kAccelerometerMaxRange =
        kAccelerometerScale * 32768.0f;

struct RingEvent {
    std::uint8_t sensor_id;
    std::uint8_t flags;
    std::int16_t x;
    std::int16_t y;
    std::int16_t z;
    std::int64_t timestamp_ns;
};

struct AndroidAccelEvent {
    float x;
    float y;
    float z;
    std::int64_t timestamp_ns;
};

struct SamplingConfig {
    std::int64_t period_ns;
    std::int64_t max_report_latency_ns;
    std::int32_t odr_millihz;
    std::int32_t ec_rate_ms;
};

bool DecodeRingEvent(const std::uint8_t* bytes, std::size_t size,
                     RingEvent* event);
AndroidAccelEvent ConvertAccelerometer(const RingEvent& event);
SamplingConfig MakeSamplingConfig(std::int64_t requested_period_ns,
                                  std::int64_t requested_latency_ns);

}  // namespace pixelc::sensors
