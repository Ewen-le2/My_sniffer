#include "analysis.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <utility>

constexpr  std::chrono::seconds WINDOW = std::chrono::seconds(10);
constexpr  std::chrono::seconds ALERT_COOLDOWN = std::chrono::seconds(10);

constexpr std::size_t PACKET_THRESHOLD = 500;
constexpr std::size_t SERVICE_THRESHOLD = 100;

struct ServiceKey {
    std::string destination;
    uint16_t port;
    uint8_t protocol;

    bool operator==(const ServiceKey& other) const {
        return destination == other.destination &&
               port == other.port &&
               protocol == other.protocol;
    }
};

struct ServiceKeyHash {
    std::size_t operator()(const ServiceKey& key) const {
        std::size_t h = std::hash<std::string>{}(key.destination);
        h ^= std::hash<unsigned int>{}(key.port)+ 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<unsigned int>{}(key.protocol)+ 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};

struct ServiceState {
    std::deque<std::chrono::system_clock::time_point> times;
    std::chrono::system_clock::time_point last_alert{};
};

struct TrafficAnalyzer::Impl {
    std::deque<std::chrono::system_clock::time_point> packet_times;
    std::unordered_map<ServiceKey, ServiceState, ServiceKeyHash> services;
    std::chrono::system_clock::time_point last_spike_alert{};
};

TrafficAnalyzer::TrafficAnalyzer() : impl_(std::make_unique<Impl>()) {}

TrafficAnalyzer::~TrafficAnalyzer() = default;

std::vector<AnalysisAlert> TrafficAnalyzer::process(const ETH_packet& packet)
{
    std::vector<AnalysisAlert> result;
    const std::chrono::system_clock::time_point now = packet.time;

    // Detection 1: overall TRAFFIC_SPIKE
    impl_->packet_times.push_back(now);

    while (!impl_->packet_times.empty() && now >= impl_->packet_times.front() && now - impl_->packet_times.front() > WINDOW) {
        impl_->packet_times.pop_front();
    }

    if (impl_->packet_times.size() > PACKET_THRESHOLD) {
        if (impl_->last_spike_alert.time_since_epoch().count() == 0 || now - impl_->last_spike_alert >= ALERT_COOLDOWN) {
            result.push_back({
                now,
                AlertSeverity::Warning,
                "TRAFFIC_SPIKE",
                "Observed " +
                    std::to_string(impl_->packet_times.size()) +
                    " packets in the last 10 seconds.",
                "",
                ""
            });
            impl_->last_spike_alert = now;
        }
    }

    // Dection 2: SERVICE_RATE
    if (ntohs(packet.hdr.ethertype) != ETH_T_IPV4)
        return result;

    std::optional<IPV4_packet> ipv4 = parse_ipv4(const_cast<ETH_packet*>(&packet));

    if (!ipv4)
        return result;

    const uint8_t protocol = ipv4->hdr.Protocol;

    uint16_t destination_port = 0;

    if (protocol == Proto_TCP) {
        std::optional<TCP_packet> tcp = parse_tcp(&*ipv4);

        if (!tcp)
            return result;

        destination_port = ntohs(*reinterpret_cast<const uint16_t*>(tcp->hdr.dst_port));
    }
    else if (protocol == Proto_UDP) {
        std::optional<UDP_packet> udp = parse_udp(&*ipv4);

        if (!udp)
            return result;

        destination_port = ntohs(*reinterpret_cast<const uint16_t*>(udp->hdr.dst_port));
    }
    else {
        return result;
    }

    const std::string source = ipv4_to_string(ipv4->hdr.SourceIP);
    const std::string destination = ipv4_to_string(ipv4->hdr.DestinationIP);
    ServiceKey key{
        destination,
        destination_port,
        protocol
    };

    ServiceState& state = impl_->services[key];
    state.times.push_back(now);

    while (!state.times.empty() && now >= state.times.front() && now - state.times.front() > WINDOW) {
        state.times.pop_front();
    }

    if (state.times.size() > SERVICE_THRESHOLD) {
        if (state.last_alert.time_since_epoch().count() == 0 || now - state.last_alert >= ALERT_COOLDOWN) {

            const std::string protocol_name = protocol == Proto_TCP ? "TCP" : "UDP";

            result.push_back({
                now,
                AlertSeverity::Warning,
                "SERVICE_RATE",
                std::to_string(state.times.size()) +
                    " packets to " + destination + ":" +
                    std::to_string(destination_port) +
                    " over the last 10 seconds (" +
                    protocol_name + ").",
                source,
                destination
            });

            state.last_alert = now;
        }
    }

    return result;
}