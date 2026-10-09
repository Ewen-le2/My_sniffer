#include "filter.hpp"
#include "protocol.hpp"

bool filter_port(const ETH_packet& eth, uint16_t wanted, Direction direction)
{
    std::optional<IPV4_packet> ipv4 = parse_ipv4(const_cast<ETH_packet*>(&eth));

    if (!ipv4)
        return false;

    if (ipv4->hdr.Protocol == Proto_TCP) {
        std::optional<TCP_packet> tcp = parse_tcp(&*ipv4);
        if (!tcp) return false;

        return matches_port(read_port(tcp->hdr.src_port), read_port(tcp->hdr.dst_port), wanted, direction);
    }

    if (ipv4->hdr.Protocol == Proto_UDP) {
        std::optional<UDP_packet> udp = parse_udp(&*ipv4);
        if (!udp) return false;

        return matches_port(read_port(udp->hdr.src_port), read_port(udp->hdr.dst_port), wanted, direction);
    }

    return false;
}