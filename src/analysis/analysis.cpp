#include "analysis.hpp"

#include <iostream>
#include <utility>

constexpr std::size_t MAX_ALERTS = 1000;

Analysis::Analysis() = default;

Analysis::~Analysis() = default;

void Analysis::process(const ETH_packet& packet)
{
    std::vector<AnalysisAlert> traffic_alerts = traffic_.process(packet);
    std::vector<AnalysisAlert> beacon_alerts = beacon_.process(packet);

    for (AnalysisAlert& alert : traffic_alerts)
        alerts_.push_back(std::move(alert));

    for (AnalysisAlert& alert : beacon_alerts)
        alerts_.push_back(std::move(alert));

    if (alerts_.size() > MAX_ALERTS) {
        const std::size_t excess =
            alerts_.size() - MAX_ALERTS;

        alerts_.erase(
            alerts_.begin(),
            alerts_.begin() +
                static_cast<std::ptrdiff_t>(excess));
    }
}

const std::vector<AnalysisAlert>& Analysis::alerts() const
{
    return alerts_;
}

void Analysis::clear_alerts()
{
    alerts_.clear();
}

void Analysis::print_alerts() const
{
    if (alerts_.empty()) {
        std::cout << "No analysis alerts.\n";
        return;
    }

    for (const AnalysisAlert& alert : alerts())
        std::cout << format_alert(alert) << "\n\n";
}