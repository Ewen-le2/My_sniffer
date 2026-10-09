#include "filter.hpp"
#include "protocol.hpp"

std::optional<std::array<uint8_t, 4>> parse_filter_ip(const std::string& text)
{
    std::array<uint8_t, 4> result{};
    std::istringstream input(text);
    std::string part;

    for (std::size_t i = 0; i < 4; ++i) {
        if (!std::getline(input, part, '.') || part.empty() || !std::all_of(part.begin(), part.end(), [](unsigned char c) {return c >= '0' && c <= '9';}))
            return std::nullopt;
        try {
            std::size_t pos = 0;
            unsigned long n = std::stoul(part, &pos);

            if (pos != part.size() || n > 255)
                return std::nullopt;

            result[i] = static_cast<uint8_t>(n);
        } catch (...) {
            return std::nullopt;
        }
    }

    if (std::getline(input, part, '.'))
        return std::nullopt;

    return result;
}

bool filter_ip(const ETH_packet& eth, const std::array<uint8_t, 4>& wanted, Direction direction)
{
    std::optional<IPV4_packet> ipv4 = parse_ipv4(const_cast<ETH_packet*>(&eth));

    if (!ipv4)
        return false;

    return matches_address(ipv4->hdr.SourceIP, ipv4->hdr.DestinationIP, wanted.data(), wanted.size(), direction);
}