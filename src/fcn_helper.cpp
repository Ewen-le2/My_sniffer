#include <ostream>
#include <iomanip>
#include <format>
#include "fcn_helper.hpp"
#include "protocol.hpp"
#include "protocol_parsing/ethernet.hpp"


//format mac address to print in hexa
std::ostream& print_mac(std::ostream& os, const uint8_t *mac) {
    os << std::format(
        "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return os;
}

//overload to print eth_header structure
std::ostream& operator<<(std::ostream& os, const ETH_header& h) {
    os << "Ethernet Header { "
        << "dst="; print_mac(os, h.dst)
        << ", src="; print_mac(os, h.src)
        << ", ethertype=0x"
        << std::hex << std::setw(4) << std::setfill('0')
        << ntohs(h.ethertype)
        << std::dec
        << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const ETH_packet& h)
{
    const std::time_t time_t_value = std::chrono::system_clock::to_time_t(h.time);
    os << "Time: "
        << std::put_time(std::localtime(&time_t_value), "%a %b %d %H:%M:%S")
        << '.'
        << std::setfill('0')
        << std::setw(6)
        << duration_cast<std::chrono::microseconds>(h.time - time_point_cast<std::chrono::seconds>(h.time)).count()
        << std::setfill(' ')
        << '\t'
        << h.hdr
        << ' '
        << h.payload.size()
        << '\n';
    return os;
}

std::ostream& print_ipv4(std::ostream& os, const uint8_t *ip) {
    os << std::format(
        "{}.{}.{}.{}\n",
        ip[0], ip[1], ip[2], ip[3]);
    return os;
}

void printHex(const unsigned char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

void print_time(ETH_packet &h) {
    const std::time_t time_t_value = std::chrono::system_clock::to_time_t(h.time);
    std::cout
        << "Time: "
        << std::put_time(std::localtime(&time_t_value), "%a %b %d %H:%M:%S")
        << '.'
        << std::setfill('0')
        << std::setw(6)
        << duration_cast<std::chrono::microseconds>(h.time - time_point_cast<std::chrono::seconds>(h.time)).count();
}