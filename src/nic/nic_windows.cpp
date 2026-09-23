#include "pcap.h"
#include "nic.hpp"

NIC_Windows::NIC_Windows() : pcap(nullptr) {}

NIC_Windows::~NIC_Windows() {
    close();
}

bool NIC_Windows::open(const std::string& name)
{
    pcap_t* _pcap = reinterpret_cast<pcap_t *>(pcap);
    char errbuf[PCAP_ERRBUF_SIZE]{};

    _pcap = pcap_open_live(
        name.c_str(),
        65535,  // snaplen
        1,      // promiscuous
        1000,   // timeout ms
        errbuf
    );

    if (_pcap == nullptr) {
        printf("pcap_open_live failed: %s\n", errbuf);
        return false;
    }
    pcap = _pcap;
    return true;
}

std::ptrdiff_t NIC_Windows::read(uint8_t* buffer, size_t len)
{
    pcap_t* _pcap = pcap;
    if (_pcap == nullptr || buffer == nullptr) {
        printf("machin\n");
        return -1;
    }

    pcap_pkthdr* header = nullptr;
    const u_char* packet = nullptr;

    int result = pcap_next_ex(
        _pcap,
        &header,
        &packet
    );

    if (result == 0) {
        // Timeout, no packet.
        return 0;
    }

    if (result < 0) {
        printf("pcap_next_ex failed: %s\n",
               pcap_geterr(_pcap));
        return -1;
    }

    if (header->caplen > len) {
        printf("Packet too large: %u bytes\n",
               header->caplen);
        return -1;
    }

    std::memcpy(
        buffer,
        packet,
        header->caplen
    );

    return static_cast<std::ptrdiff_t>(
        header->caplen
    );
}


void NIC_Windows::close() {
    pcap_close(pcap);
    return;
}
