#include "filter.hpp"

std::optional<std::array<uint8_t, 6>> parse_filter_mac(const std::string& text)
{
    if (text.size() != 17)
        return std::nullopt;

    std::array<uint8_t, 6> result{};

    for (std::size_t i = 0; i < 6; ++i) {
        const std::size_t offset = i * 3;

        if (i != 0 && text[offset - 1] != ':')
            return std::nullopt;

        unsigned int value = 0;
        std::istringstream input(text.substr(offset, 2));
        input >> std::hex >> value;

        if (!input || !input.eof() || value > 255)
            return std::nullopt;

        result[i] = static_cast<uint8_t>(value);
    }

    return result;
}

bool filter_mac(const ETH_packet& eth, const std::array<uint8_t, 6>& wanted, Direction direction)
{
    return matches_address(eth.hdr.src, eth.hdr.dst, wanted.data(), wanted.size(), direction);
}