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
#include "analysis.hpp"
#include "version.hpp"

void help(void) {
    std::cout << "q          Exit app\n";
    std::cout << "h          Print this page\n";
    std::cout << "v          Print the app version\n";
    std::cout << "s          Print saved hisotry\n";
    std::cout << "a          Show analysis alerts\n";
    std::cout << "f          Enter filter mode\n";
    std::cout << "\nFilter commands:\n";
    std::cout << "   proto <ipv4|tcp|udp>     Filter by protocol\n";
    std::cout << "   port <number>            Match source OR destination port\n";
    std::cout << "   port <number> in         Match destination port only\n";
    std::cout << "   port <number> out        Match source port only\n";
    std::cout << "   ip <address>             Match source OR destination IPv4\n";
    std::cout << "   ip <address> in          Match destination IPv4 only\n";
    std::cout << "   ip <address> out         Match source IPv4 only\n";
    std::cout << "   mac <address>            Match source OR destination MAC\n";
    std::cout << "   mac <address> in         Match destination MAC only\n";
    std::cout << "   mac <address> out        Match source MAC only\n";
    std::cout << "\nExamples:\n";
    std::cout << "   proto tcp\n";
    std::cout << "   port 443\n";
    std::cout << "   port 53 in\n";
    std::cout << "   ip 192.168.1.10 out\n";
    std::cout << "   mac aa:bb:cc:dd:ee:ff in\n";
}

void print_history(std::deque<ETH_packet> history) {
    for (const ETH_packet& eth : history)
        print_all(eth);
}

void print_version() {
    std::cout << "Version: " << SNIFFER_VERSION << std::endl;
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
    Analysis analysis;

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
                if (c == 's') print_history(history);
                if (c == 'a') analysis.print_alerts();
                if (c == 'v') print_version();
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
                        std::cout << "\b" << std::flush;
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
        analysis.process(history.back());
    }
    return 0;
}