#include "protocol.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <optional>

std::optional<ETH_packet> parse_ethernet(uint8_t* buffer, size_t bytes) {
    if (bytes < sizeof(ETH_header))
        return std::nullopt;
    ETH_packet pck{};
    pck.time = std::chrono::system_clock::now();
    memcpy(&pck.hdr, buffer, sizeof(pck.hdr));
    pck.payload.assign(buffer + sizeof(pck.hdr), buffer + bytes);
    return pck;
};
