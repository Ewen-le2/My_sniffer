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


#include "header.hpp"

std::ostream& print_mac(std::ostream& os, const uint8_t *mac);

std::ostream& operator<<(std::ostream& os, const eth_header& h);
