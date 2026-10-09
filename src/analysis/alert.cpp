#include "analysis.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

const char* severity_name(AlertSeverity severity)
{
    switch (severity) {
        case AlertSeverity::Info:
            return "INFO";

        case AlertSeverity::Warning:
            return "WARNING";

        case AlertSeverity::Critical:
            return "CRITICAL";
    }

    return "UNKNOWN";
}

std::string format_alert(const AnalysisAlert& alert)
{
    const std::time_t timestamp =
        std::chrono::system_clock::to_time_t(alert.time);

    std::ostringstream output;

    output << '['
           << std::put_time(
                std::localtime(&timestamp),
                "%Y-%m-%d %H:%M:%S")
           << "] ["
           << severity_name(alert.severity)
           << "] ["
           << alert.rule
           << "] "
           << alert.description;

    if (!alert.source_ip.empty())
        output << "\n    Source: " << alert.source_ip;

    if (!alert.destination_ip.empty())
        output << "\n    Destination: " << alert.destination_ip;

    return output.str();
}