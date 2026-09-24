#include "protocol.hpp"

std::optional<UDP_packet> parse_udp(IPV4_packet *pck) {
    if (pck->payload_len < sizeof(UDP_packet))
        return std::nullopt;
    UDP_header *hdr = reinterpret_cast<struct UDP_header*>(pck->payload);
    UDP_packet data{};
    data.hdr = *hdr;
    data.payload = pck->payload + sizeof(UDP_header);
    data.payload_len = pck->payload_len - sizeof(UDP_header);
    return data;
}