#include "nic.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <net/if.h>
#include <netpacket/packet.h>


NIC_Linux::NIC_Linux() : _fd(-1) {}

NIC_Linux::~NIC_Linux() {
    close();
}

bool NIC_Linux::open(const std::string& name) {
    //use name for nic name (eth0, wlo1, etc...) or "all";
    _fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (_fd < 0) {
        perror("socket");
        return false;
    }

    unsigned int ifindex = 0;

    if (name != "all") {
        ifindex = if_nametoindex(name.c_str());
        if (ifindex == 0) {
            std::cerr << "Interface not found: " << name << "\n";
            ::close(_fd);
            _fd = -1;
            return false;
        }
    }
    struct sockaddr_ll sll{};
    sll.sll_family   = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    sll.sll_ifindex  = ifindex;

    if (bind(_fd, reinterpret_cast<struct sockaddr*>(&sll), sizeof(sll)) < 0) {
        std::cerr << "Error binding to interface " << name
                  << ": " << strerror(errno) << "\n";
        ::close(_fd);
        _fd = -1;
        return false;
    }

    std::cout << "Bound raw socket to interface " << name
              << " (index " << ifindex << ")\n";


    int sockflags = fcntl(_fd, F_GETFL, 0);
    fcntl(_fd, F_SETFL, sockflags | O_NONBLOCK);

    if (setsockopt(_fd, SOL_SOCKET, SO_BINDTODEVICE, name.c_str(), strlen(name.c_str())) < 0) {
        perror("Error Socket bind");
        _fd = -1;
        exit(false);
    }
    return true;
}

std::ptrdiff_t NIC_Linux::read(uint8_t* buffer, size_t len) {
    return recvfrom(_fd, buffer, len, 0, nullptr, nullptr);
}


void NIC_Linux::close() {
    if (_fd >= 0) {
        ::close(_fd);
        _fd = -1;
    }
}
