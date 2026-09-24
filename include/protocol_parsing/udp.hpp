#pragma once

#include "protocol.hpp"
#include <cstdint>
#include <optional>

struct UDP_header {
    uint8_t src_port[2];
    uint8_t dst_port[2];
    uint8_t length[2];
    uint8_t checksum[2];
};

struct UDP_packet {
    struct UDP_header hdr;
    uint8_t *payload;
    uint16_t payload_len;
};

std::optional<UDP_packet> parse_udp(IPV4_packet *);