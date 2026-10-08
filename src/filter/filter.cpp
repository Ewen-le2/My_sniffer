#include "filter.hpp"
#include "protocol_parsing/ethernet.hpp"
#include "protocol_parsing/ipv4.hpp"

void execute_filter(const std::string& command, const std::deque<ETH_packet>& history)
{
    std::stringstream ss(command);

    std::string filter;
    std::string value;

    ss >> filter >> value;

    if (filter == "proto")
    {
        for (const ETH_packet& eth : history)
        {
            std::optional<IPV4_packet> ipv4 = parse_ipv4( const_cast<ETH_packet*>(&eth));

            if (!ipv4)
                continue;

            bool match = false;

            if (value == "ipv4")
                match = true;

            else if (value == "tcp")
                match = ipv4->hdr.Protocol == Proto_TCP;

            else if (value == "udp")
                match = ipv4->hdr.Protocol == Proto_UDP;

            if (match)
            {
                print_time(*const_cast<ETH_packet*>(&eth));
                std::cout << eth.hdr << '\n';
            }
        }
    }
    else if (filter == "port")
    {
        uint16_t wanted = static_cast<uint16_t>(std::stoi(value));

        for (const ETH_packet& eth : history)
        {
            std::optional<IPV4_packet> ipv4 = parse_ipv4(const_cast<ETH_packet*>(&eth));

            if (!ipv4)
                continue;

            if (ipv4->hdr.Protocol != Proto_TCP)
                continue;

            auto tcp = parse_tcp(&*ipv4);

            if (!tcp)
                continue;

            uint16_t src = ntohs(*reinterpret_cast<const uint16_t*>(tcp->hdr.src_port));

            uint16_t dst = ntohs(*reinterpret_cast<const uint16_t*>(tcp->hdr.dst_port));

            if (src == wanted || dst == wanted)
            {
                print_time(*const_cast<ETH_packet*>(&eth));
                std::cout << eth.hdr << '\n';
            }
        }
    }
    else
    {
        std::cout
            << "Unknown filter\n";
    }
}