#include "analysis.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <functional>

constexpr std::chrono::seconds MIN_INTERVAL = std::chrono::seconds(2);
constexpr std::chrono::minutes MAX_INTERVAL = std::chrono::minutes(5);
constexpr std::chrono::minutes ALERT_COOLDOWN = std::chrono::minutes(1);

constexpr std::size_t REQUIRED_INTERVALS = 5;
constexpr double MAX_RELATIVE_DEVIATION = 0.20;

struct NetFlowKey {
    std::string source;
    std::string destination;
    uint16_t source_port;
    uint16_t destination_port;
    uint8_t protocol;

    bool operator==(const NetFlowKey& other) const {
        return source == other.source &&
               destination == other.destination &&
               source_port == other.source_port &&
               destination_port == other.destination_port &&
               protocol == other.protocol;
    }
};

struct NetFlowKeyHash {
    std::size_t operator()(const NetFlowKey& key) const {
        std::size_t h = std::hash<std::string>{}(key.source);

        std::function<void(std::size_t)> combine = [&h](std::size_t value) {
            h ^= value + 0x9e3779b9 + (h << 6) + (h >> 2);
        };

        combine(std::hash<std::string>{}(key.destination));
        combine(std::hash<unsigned int>{}(key.source_port));
        combine(std::hash<unsigned int>{}(key.destination_port));
        combine(std::hash<unsigned int>{}(key.protocol));

        return h;
    }
};

struct NetFlowState {
    std::chrono::system_clock::time_point last_seen{};
    std::chrono::system_clock::time_point last_alert{};
    std::deque<double> intervals;
};

struct BeaconDetector::Impl {
    std::unordered_map<NetFlowKey, NetFlowState, NetFlowKeyHash> flows;
};

BeaconDetector::BeaconDetector() : impl_(std::make_unique<Impl>()) {}

BeaconDetector::~BeaconDetector() = default;

std::vector<AnalysisAlert> BeaconDetector::process(const ETH_packet& packet)
{
    std::vector<AnalysisAlert> result;

    if (ntohs(packet.hdr.ethertype) != ETH_T_IPV4)
        return result;

    std::optional<IPV4_packet> ipv4 = parse_ipv4(
        const_cast<ETH_packet*>(&packet));

    if (!ipv4)
        return result;

    uint16_t source_port = 0;
    uint16_t destination_port = 0;

    if (ipv4->hdr.Protocol == Proto_TCP) {
        std::optional<TCP_packet> tcp = parse_tcp(&*ipv4);

        if (!tcp)
            return result;

        source_port = ntohs(*reinterpret_cast<const uint16_t*>(tcp->hdr.src_port));
        destination_port = ntohs(*reinterpret_cast<const uint16_t*>(tcp->hdr.dst_port));
    }
    else if (ipv4->hdr.Protocol == Proto_UDP) {
        std::optional<UDP_packet> udp = parse_udp(&*ipv4);

        if (!udp)
            return result;

        source_port = ntohs(*reinterpret_cast<const uint16_t*>(udp->hdr.src_port));
        destination_port = ntohs(*reinterpret_cast<const uint16_t*>(udp->hdr.dst_port));
    }
    else {
        return result;
    }

    const  std::chrono::system_clock::time_point now = packet.time;

    NetFlowKey key{
        ipv4_to_string(ipv4->hdr.SourceIP),
        ipv4_to_string(ipv4->hdr.DestinationIP),
        source_port,
        destination_port,
        ipv4->hdr.Protocol
    };

    NetFlowState& state = impl_->flows[key];

    if (state.last_seen.time_since_epoch().count() != 0) {
        if (now <= state.last_seen)
            return result;

        const auto elapsed = now - state.last_seen;

        if (elapsed >= MIN_INTERVAL && elapsed <= MAX_INTERVAL) {
            state.intervals.push_back(std::chrono::duration<double>(elapsed).count());

            while (state.intervals.size() > 8)
                state.intervals.pop_front();
        }
        else if (elapsed > MAX_INTERVAL) {
            state.intervals.clear();
        }
    }

    state.last_seen = now;

    if (state.intervals.size() < REQUIRED_INTERVALS)
        return result;

    double total = 0.0;

    for (double interval : state.intervals)
        total += interval;

    const double average = total / static_cast<double>(state.intervals.size());

    if (average <= 0.0)
        return result;

    double maximum_deviation = 0.0;

    for (double interval : state.intervals) {
        const double deviation = std::abs(interval - average) / average;
        maximum_deviation = (std::max)(maximum_deviation, deviation);
    }

    if (maximum_deviation > MAX_RELATIVE_DEVIATION)
        return result;

    if (state.last_alert.time_since_epoch().count() != 0 && now - state.last_alert < ALERT_COOLDOWN) {
        return result;
    }

    const std::string protocol_name = key.protocol == Proto_TCP ? "TCP" : "UDP";

    result.push_back({
        now,
        AlertSeverity::Warning,
        "REGULAR_BEACONING",
        "Repeated " + protocol_name +
            " communication with an average interval of " +
            std::to_string(average) +
            " seconds; interval deviation is within 20%.",
        key.source,
        key.destination
    });

    state.last_alert = now;

    return result;
}