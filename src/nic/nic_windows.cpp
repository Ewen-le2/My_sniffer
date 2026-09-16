#include "nic.hpp"
#include <cstddef>
#include <cstdint>

NIC_Windows::NIC_Windows() : _fd(-1) {}

NIC_Windows::~NIC_Windows() {
    close();
}

bool NIC_Windows::open(const std::string& name) {
    return 0;
}

ssize_t NIC_Windows::read(uint8_t* buffer, size_t len) {
    return 0;
}

void NIC_Windows::close() {
    return;
}

int NIC_Windows::getfd() {
    return _fd;
}
