#include <ostream>
#include <iomanip>
#include <format>
#include "header.hpp"
#include "fcn_helper.hpp"


//format mac address to print in hexa
std::ostream& print_mac(std::ostream& os, const uint8_t *mac) {
    os << std::format(
        "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return os;
}

//overload to print eth_header structure
std::ostream& operator<<(std::ostream& os, const eth_header& h) {
    os << "Ethernet Header { "
       << "dst="; print_mac(os, h.dst);
    os << ", src="; print_mac(os, h.src);
    os << ", ethertype=0x"
       << std::hex << std::setw(4) << std::setfill('0')
       << ntohs(h.ethertype)
       << std::dec
       << " }";
    return os;
}