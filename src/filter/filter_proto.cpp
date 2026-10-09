#include "filter.hpp"
#include "protocol.hpp"

bool filter_proto(const ETH_packet& eth, const std::string& value)
{
    if (ntohs(eth.hdr.ethertype) != ETH_T_IPV4)
        return false;

    std::optional<IPV4_packet> ipv4 = parse_ipv4(const_cast<ETH_packet*>(&eth));

    if (!ipv4)
        return false;

    if (value == "ipv4")
        return true;

    if (value == "tcp")
        return ipv4->hdr.Protocol == Proto_TCP;

    if (value == "udp")
        return ipv4->hdr.Protocol == Proto_UDP;

    return false;
}