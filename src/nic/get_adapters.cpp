#include "get_adapters.hpp"
#include <iostream>

#ifndef _WIN32
    #include <sys/types.h>
    #include <ifaddrs.h>
    #include <sys/socket.h>
#endif

#ifdef _WIN32
    #include "pcap.h"
#endif

int list_adapters () {

    #ifdef _WIN32
        return listWindowsAdapters();
    #else
        return listLinuxAdapters();
    #endif

}

#ifdef _WIN32
    int listWindowsAdapters()
    {
        pcap_if_t* alldevs;
        char errbuf[PCAP_ERRBUF_SIZE];

        if (pcap_findalldevs(&alldevs, errbuf) == -1) {
            std::cerr << "Can't find Network Card: " << errbuf << std::endl;
            return false;
        }

        for (pcap_if_t* dev = alldevs; dev != nullptr; dev = dev->next)
        {
            std::cout << "Name:\t\t" << dev->name << std::endl;
            std::cout << "Description:\t" << dev->description << std::endl;
            std::cout << "Addresses:\t" << dev->addresses << std::endl;
            std::cout << "Flags:\t\t" << dev->flags << std::endl << std::endl;
        }

        pcap_freealldevs(alldevs);
        return true;
    }
#else
    int listLinuxAdapters() {
        ifaddrs *addr;
        getifaddrs(&addr);
        ifaddrs *tmp = addr;
        while (tmp != nullptr) {
            if (tmp->ifa_addr->sa_family == AF_PACKET) {
                std::cout << tmp->ifa_name << std::endl;
            }
            tmp = tmp->ifa_next;
        };
        freeifaddrs(addr);
        return true;
    }
#endif