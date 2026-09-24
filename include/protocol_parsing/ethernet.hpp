#pragma once

#include "protocol.hpp"
#include <cstddef>
#include <cstdint>
#include <chrono>
#include <vector>
#include <optional>

#define ETH_T_IPV4  0x0800
#define ETH_T_IPV6  0x86DD
#define ETH_T_ARP   0x0806
#define ETH_T_VLAN  0x8100

struct ETH_header {
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t ethertype;
};

struct ETH_packet {
    std::chrono::system_clock::time_point time;
    struct ETH_header hdr;
    std::vector<uint8_t>payload;
};

std::optional<ETH_packet> parse_ethernet(uint8_t *buffer, size_t bytes);