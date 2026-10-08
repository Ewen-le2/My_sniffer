#include "fcn_helper.hpp"
#include "nic.hpp"
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <deque>
#include <ostream>
#include "get_adapters.hpp"
#include "keyboard.hpp"
#include "protocol.hpp"
#include "filter.hpp"
#include "protocol_parsing/ipv4.hpp"
#include "protocol_parsing/udp.hpp"


void help(void) {
    std::cout << "q \t Exit app\n";
    std::cout << "h \t print this page\n";
    std::cout << "f \t enter filer mode\n";
    std::cout << "    Filter mode function:\n";
    std::cout << "\t proto \t ipv4, tcp, udp\n";
    std::cout << "\t port \t tcp, udp port\n";
}

int main(void)
{
    uint8_t buffer [65536]; //buffer only for reading raw packet
    std::unique_ptr<NIC> nic;
    std::unique_ptr<NIC_Factory> nic_factory = std::make_unique<NIC_Factory>();

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
    constexpr std::size_t MAX_HISTORY = 5000;
    std::deque<ETH_packet> history;

    std::cout << "Press 'h' for more command\n";
    std::cout << "Press q to exit: \n" << std::endl;
    std::string command;
    bool command_mode = false;
    while (true) {
        char c;
        if (keyboard.read(c)) {
            if (!command_mode)
            {
                if (c == 'q') break;
                if (c == 'h') help();
                if (c == 'f') {
                    command_mode = true;
                    command.clear();

                    std::cout << "\nfilter> ";
                    std::flush(std::cout);
                }
            }
            else
            {
                if (c == '\b' || c == 127) {
                    if (!command.empty()) {
                        command.pop_back();
                        std::cout << "\b \b" << std::flush;
                    }
                }
                if (c == '\n' || c == '\r') {
                    execute_filter(command, history);

                    command_mode = false;
                    std::cout << "\n";
                } else {
                    command += c;
                    std::cout << c << std::flush;
                }
            }
        }
        std::ptrdiff_t bytes = nic->read(buffer, sizeof(buffer));
        if (bytes == 0) continue;
        if (bytes == -1) continue;

        std::optional<ETH_packet> packet = parse_ethernet(buffer, bytes);
        if (!packet.has_value()) continue;
        history.push_back(std::move(*packet));
        while (history.size() > MAX_HISTORY)
            history.pop_front();
        if (ntohs(packet->hdr.ethertype) == ETH_T_IPV4) {
            std::optional<IPV4_packet> ipv4 = parse_ipv4(&history.back());
            if (ipv4.has_value() && ipv4->hdr.Protocol == Proto_UDP) {
                std::optional<UDP_packet> udp = parse_udp(&ipv4.value());
                if (udp.has_value()) {
                    uint16_t src = ntohs(*reinterpret_cast<const uint16_t*>(udp->hdr.src_port));
                    uint16_t dst = ntohs(*reinterpret_cast<const uint16_t*>(udp->hdr.dst_port));
                    std::cout << "src = " << src << "dst = " << dst << std::endl;
                }
            }
        }
    }
    return 0;
}