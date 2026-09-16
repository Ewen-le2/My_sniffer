#include "nic.hpp"
#include <fcntl.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

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

    int sockflags = fcntl(_fd, F_GETFL, 0);
    fcntl(_fd, F_SETFL, sockflags | O_NONBLOCK);

    return true;
}

ssize_t NIC_Linux::read(uint8_t* buffer, size_t len) {
    return recvfrom(_fd, buffer, len, 0, nullptr, nullptr);
}

void NIC_Linux::close() {
    if (_fd >= 0) {
        ::close(_fd);
        _fd = -1;
    }
}

int NIC_Linux::getfd() {
    return _fd;
}
