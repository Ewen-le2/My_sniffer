#include "protocol.hpp"
#include <iostream>

std::optional<IPV4_packet> parse_ipv4(ETH_packet *pck) {
    if (pck->payload.size() < sizeof(IPV4_header))
        return std::nullopt;
    IPV4_header *ipv4 = reinterpret_cast<struct IPV4_header *>(pck->payload.data());
    IPV4_packet data{};
    data.hdr = *ipv4;
    data.payload = pck->payload.data() + sizeof(IPV4_header);
    data.payload_len = pck->payload.size() - sizeof(IPV4_header);
    return data;
}