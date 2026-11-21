#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "pixelc_sensors/core.h"

namespace pixelc::sensors {

struct IioTopology {
    std::string provider_name;
    std::string provider_path;
    std::string ring_name;
    std::string ring_path;
    std::string trigger_name;
    std::string ring_device_path;
};

class IioEngine {
  public:
    explicit IioEngine(std::string sysfs_root = "/sys/bus/iio/devices",
                       std::string dev_root = "/dev");
    ~IioEngine();

    IioEngine(const IioEngine&) = delete;
    IioEngine& operator=(const IioEngine&) = delete;

    bool Discover(std::string* error);
    bool Prepare(std::string* error);
    bool Activate(bool enabled, std::string* error);
    bool SetSampling(std::int64_t period_ns, std::int64_t latency_ns,
                     std::string* error);
    int Read(std::vector<AndroidAccelEvent>* events, std::size_t max_events,
             std::string* error);
    void Restore();

    const IioTopology& topology() const { return topology_; }
    bool enabled() const { return enabled_; }
    int fd() const { return ring_fd_; }

  private:
    bool Write(const std::string& path, const std::string& value,
               std::string* error);
    bool ConfigureChannels(bool enabled, std::string* error);

    std::string sysfs_root_;
    std::string dev_root_;
    IioTopology topology_;
    SamplingConfig sampling_;
    int ring_fd_ = -1;
    bool prepared_ = false;
    bool enabled_ = false;
    bool saved_state_valid_ = false;
    std::string saved_frequency_;
    std::string saved_sampling_frequency_;
    std::string saved_buffer_enable_;
    std::string saved_buffer_length_;
    std::string saved_trigger_;
    std::vector<std::pair<std::string, std::string>> saved_channel_states_;
};

}  // namespace pixelc::sensors
