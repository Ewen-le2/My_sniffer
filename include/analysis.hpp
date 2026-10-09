#pragma once

#include "protocol.hpp"
#include "fcn_helper.hpp"

#include <chrono>
#include <cstdint>
#include <deque>
#include <memory>
#include <string>
#include <vector>

enum class AlertSeverity {
    Info,
    Warning,
    Critical
};

struct AnalysisAlert {
    std::chrono::system_clock::time_point time;
    AlertSeverity severity;
    std::string rule;
    std::string description;
    std::string source_ip;
    std::string destination_ip;
};

class TrafficAnalyzer {
public:
    TrafficAnalyzer();
    ~TrafficAnalyzer();

    TrafficAnalyzer(const TrafficAnalyzer&) = delete;
    TrafficAnalyzer& operator=(const TrafficAnalyzer&) = delete;

    std::vector<AnalysisAlert> process(const ETH_packet& packet);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class BeaconDetector {
public:
    BeaconDetector();
    ~BeaconDetector();

    BeaconDetector(const BeaconDetector&) = delete;
    BeaconDetector& operator=(const BeaconDetector&) = delete;

    std::vector<AnalysisAlert> process(const ETH_packet& packet);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class Analysis {
public:
    Analysis();
    ~Analysis();

    Analysis(const Analysis&) = delete;
    Analysis& operator=(const Analysis&) = delete;

    // Call once for every captured packet.
    void process(const ETH_packet& packet);

    const std::vector<AnalysisAlert>& alerts() const;

    void clear_alerts();

    // Print all stored alerts.
    void print_alerts() const;

private:
    TrafficAnalyzer traffic_;
    BeaconDetector beacon_;
    std::vector<AnalysisAlert> alerts_;
};

std::string format_alert(const AnalysisAlert& alert);