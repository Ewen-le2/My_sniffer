#include "protocol.hpp"
#include "filter.hpp"
#include "fcn_helper.hpp"

std::optional<Direction> parse_direction(const std::string& text)
{
    if (text.empty()) return Direction::Any;
    if (text == "in") return Direction::In;
    if (text == "out") return Direction::Out;
    return std::nullopt;
}

uint16_t read_port(const uint8_t port[2])
{
    return (static_cast<uint16_t>(port[0]) << 8) |
            static_cast<uint16_t>(port[1]);
}

bool matches_port(uint16_t src, uint16_t dst, uint16_t wanted, Direction direction)
{
    switch (direction) {
        case Direction::In:  return dst == wanted;
        case Direction::Out: return src == wanted;
        case Direction::Any: return src == wanted || dst == wanted;
    }
    return false;
}

bool matches_address(const uint8_t* src, const uint8_t* dst, const uint8_t* wanted, std::size_t size, Direction direction)
{
    const bool source = std::equal(src, src + size, wanted);

    const bool destination = std::equal(dst, dst + size, wanted);

    switch (direction) {
        case Direction::In:  return destination;
        case Direction::Out: return source;
        case Direction::Any: return source || destination;
    }
    return false;
}

void print_all(const ETH_packet& eth)
{
    // Layer 2: Ethernet
    print_time(eth);
    std::cout << '\n' << eth.hdr << '\n';

    if (ntohs(eth.hdr.ethertype) != ETH_T_IPV4) {
        std::cout << std::endl;
        return;
    }

    // Layer 3: IPv4
    std::optional<IPV4_packet> ipv4 = parse_ipv4(const_cast<ETH_packet*>(&eth));

    if (!ipv4) {
        std::cout << std::endl;
        return;
    }

    std::cout << "  IPv4 Header { "
              << "src=";

    print_ipv4(std::cout, ipv4->hdr.SourceIP);

    std::cout << "  dst="; print_ipv4(std::cout, ipv4->hdr.DestinationIP);

    std::cout << "  protocol="
              << static_cast<unsigned int>(ipv4->hdr.Protocol)
              << " }\n";

    // Layer 4: TCP
    if (ipv4->hdr.Protocol == Proto_TCP) {
        std::optional<TCP_packet> tcp = parse_tcp(&*ipv4);

        if (!tcp) {
            std::cout << std::endl;
            return;
        }

        std::cout << "    TCP Header { "
                  << "src_port="
                  << read_port(tcp->hdr.src_port)
                  << ", dst_port="
                  << read_port(tcp->hdr.dst_port)
                  << " }\n\n";
    }
    // Layer 4: UDP
    else if (ipv4->hdr.Protocol == Proto_UDP) {
        std::optional<UDP_packet> udp = parse_udp(&*ipv4);

        if (!udp) {
            std::cout << std::endl;
            return;
        }

        std::cout << "    UDP Header { "
                  << "src_port="
                  << read_port(udp->hdr.src_port)
                  << ", dst_port="
                  << read_port(udp->hdr.dst_port)
                  << " }\n\n";
    }
}