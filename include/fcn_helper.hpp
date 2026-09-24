#pragma once

#include <cstdint>
#include <iostream>
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
#endif


#include "protocol.hpp"

std::ostream& print_mac(std::ostream& os, const uint8_t *mac);

std::ostream& operator<<(std::ostream& os, const ETH_header& h);

std::ostream& operator<<(std::ostream& os, const ETH_packet& h);

std::ostream& print_ipv4(std::ostream& os, const uint8_t *ip);

void printHex(const unsigned char *buf, size_t len);

void print_time(ETH_packet &pck);