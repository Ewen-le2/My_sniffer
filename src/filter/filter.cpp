#include "filter.hpp"
#include <optional>

std::optional<uint16_t> parse_filter_port(const std::string& text)
{
    try {
        std::size_t pos = 0;
        unsigned long n = std::stoul(text, &pos);

        if (pos != text.size() || n == 0 || n > 65535)
            return std::nullopt;

        return static_cast<uint16_t>(n);
    } catch (...) {
        return std::nullopt;
    }
}

void execute_filter(const std::string& command, const std::deque<ETH_packet>& history)
{
    std::istringstream input(command);
    std::string type, value, direction_text, extra;

    if (!(input >> type >> value)) {
        std::cout << "Usage: proto|port|ip|mac <value> [in|out]\n";
        return;
    }

    input >> direction_text;

    if (input >> extra) {
        std::cout << "Too many arguments\n";
        return;
    }

    std::optional<Direction> direction = parse_direction(direction_text);

    if (!direction) {
        std::cout << "Direction must be 'in' or 'out'\n";
        return;
    }

    if (type == "proto" && !direction_text.empty()) {
        std::cout << "Usage: proto <ipv4|tcp|udp>\n";
        return;
    }

    if (type == "proto" && value != "ipv4" && value != "tcp" && value != "udp") {
        std::cout << "Unknown protocol\n";
        return;
    }

    std::optional<uint16_t> port = type == "port" ? parse_filter_port(value) : std::optional<uint16_t>{};

    std::optional<std::array<uint8_t, 4>> ip = type == "ip" ? parse_filter_ip(value) : std::optional<std::array<uint8_t, 4>>{};

    std::optional<std::array<uint8_t, 6>> mac = type == "mac" ? parse_filter_mac(value) : std::optional<std::array<uint8_t, 6>>{};

    if (type == "port" && !port) {
        std::cout << "Invalid port (1-65535)\n";
        return;
    }

    if (type == "ip" && !ip) {
        std::cout << "Invalid IPv4 address\n";
        return;
    }

    if (type == "mac" && !mac) {
        std::cout << "Invalid MAC address\n";
        return;
    }

    if (type != "proto" && type != "port" && type != "ip" && type != "mac") {
        std::cout << "Unknown filter: " << type << '\n';
        return;
    }

    for (const ETH_packet& eth : history) {
        bool match = false;

        if (type == "proto")
            match = filter_proto(eth, value);
        else if (type == "port")
            match = filter_port(eth, *port, *direction);
        else if (type == "ip")
            match = filter_ip(eth, *ip, *direction);
        else if (type == "mac")
            match = filter_mac(eth, *mac, *direction);

        if (match)
            print_all(eth);
    }
}