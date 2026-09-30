#include "fcn_helper.hpp"
#include "nic.hpp"
#include <iomanip>
#include <iostream>
#include <optional>
//#include <ostream>
#include <deque>
#include "get_adapters.hpp"
#include "keyboard.hpp"
#include "protocol.hpp"
#include "protocol_parsing/ethernet.hpp"


int main(void)
{
    uint8_t buffer [65536]; //buffer only for reading raw packet
    std::unique_ptr<NIC> nic;
    std::unique_ptr<NIC_Factory> nic_factory;

    list_adapters();

    nic_factory->CreateNIC(nic);

    std::cout << "Enter Card Name: " << std::endl;
    char name[256];
    if (!std::cin.get(name, sizeof(name))) {
        throw std::runtime_error("Failed to read character");
    }

    if(!nic->open(name)) {
        std::cerr << "Failed to open NIC\n";
        return 1;
    }

    Keyboard keyboard;
    std::deque<ETH_packet> history;

    std::cout << "Press q to exit: " << std::endl;
    while (true) {
        char c;
        if (keyboard.read(c)) {
            if (c == 'q' || c == 'Q')
                break;
        }
        if (keyboard.read(c)) {
            if (c == 'h') {
                for (ETH_packet packet : history)
                    print_time(packet);
                    //std::cout << packet << std::endl;
                break;
            }
        }
        std::ptrdiff_t bytes = nic->read(buffer, sizeof(buffer));
        if (bytes == 0) continue;
        if (bytes == -1) continue;

        std::optional<ETH_packet> packet = parse_ethernet(buffer, bytes);
        if (!packet.has_value()) continue;
        history.push_back(std::move(*packet));

        switch (ntohs(packet->hdr.ethertype)) {
            case ETH_T_IPV4:
                std::cout << "IPV4\n";
                std::cout << packet->hdr << std::endl;
                break;

            case ETH_T_IPV6:
                std::cout << "IPV6\n";
                std::cout << packet->hdr << std::endl;
                break;

            case ETH_T_ARP:
                std::cout << "ARP\n";
                std::cout << packet->hdr << std::endl;
                break;

            case ETH_T_VLAN:
                std::cout << "VLAN\n";
                std::cout << packet->hdr << std::endl;
                break;
            default:
                std::cout << packet->hdr << std::endl;
                std::cout << "Unknown 0x" << std::hex << std::setw(4) << std::setfill('0') << ntohs(packet->hdr.ethertype) << std::endl;
                break;
        }
    }
    return 0;
}