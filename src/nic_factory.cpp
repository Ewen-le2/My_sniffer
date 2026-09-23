#include "nic.hpp"
#include <memory>

NIC_Factory::NIC_Factory() {}

NIC_Factory::~NIC_Factory() {}

void NIC_Factory::CreateNIC(std::unique_ptr<NIC> &nic) {
    #ifdef _WIN32
        nic = std::make_unique<NIC_Windows>();
    #else
        nic = std::make_unique<NIC_Linux>();
    #endif
}