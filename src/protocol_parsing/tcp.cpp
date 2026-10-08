#include "protocol_parsing/tcp.hpp"
#include "protocol.hpp"

std::optional<TCP_packet> parse_tcp(IPV4_packet *pck) {
    if (pck->payload_len < sizeof(TCP_header))
        return std::nullopt;
    TCP_header *hdr = reinterpret_cast<struct TCP_header*>(pck->payload);
    TCP_packet data{};
    data.hdr = *hdr;
    data.payload = pck->payload + sizeof(TCP_header);
    data.payload_len = pck->payload_len - sizeof(TCP_header);
    return data;
}