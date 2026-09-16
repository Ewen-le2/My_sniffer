#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <netioapi.h>
#pragma comment(lib, "iphlpapi.lib")

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

#include "pcap.h"
#include <Packet32.h>

#include "nic.hpp"

NIC_Windows::NIC_Windows() : _fd(-1) {}

NIC_Windows::~NIC_Windows() {
    close();
}

void listWindowsAdapters()
{
    ULONG size = 0;

    DWORD result = GetAdaptersAddresses(
        AF_UNSPEC,
        GAA_FLAG_INCLUDE_PREFIX,
        nullptr,
        nullptr,
        &size
    );

    if (result != ERROR_BUFFER_OVERFLOW) {
        printf("GetAdaptersAddresses failed: %lu\n", result);
        return;
    }

    auto* adapters =
        static_cast<IP_ADAPTER_ADDRESSES*>(
            std::malloc(size)
        );

    if (!adapters) {
        return;
    }

    result = GetAdaptersAddresses(
        AF_UNSPEC,
        GAA_FLAG_INCLUDE_PREFIX,
        nullptr,
        adapters,
        &size
    );

    if (result != NO_ERROR) {
        printf("GetAdaptersAddresses failed: %lu\n", result);
        std::free(adapters);
        return;
    }

    for (auto* adapter = adapters;
         adapter != nullptr;
         adapter = adapter->Next)
    {
        wprintf(L"Name:        %s\n", adapter->FriendlyName);
        printf("GUID:        %s\n", adapter->AdapterName);
        printf("Description: %ls\n",
               adapter->Description);

        printf("IfType:      %lu\n",
               adapter->IfType);

        printf("\n");
    }

    std::free(adapters);
}

bool NIC_Windows::open(const std::string& name)
{
    pcap_t* _pcap = reinterpret_cast<pcap_t *>(pcap);
    char errbuf[PCAP_ERRBUF_SIZE]{};

    listWindowsAdapters();
    _pcap = pcap_open_live(
        name.c_str(),
        65535,   // snaplen
        1,       // promiscuous
        1000,     // timeout ms
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
    pcap_t* _pcap = reinterpret_cast<pcap_t *>(pcap);
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
    return;
}

int NIC_Windows::getfd() {
    return _fd;
}
