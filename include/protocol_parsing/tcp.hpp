#pragma once

#include "protocol.hpp"
#include <cstdint>
#include <optional>

struct TCP_header {
    uint8_t src_port[2];
    uint8_t dst_port[2];
    uint8_t sequence[4];
    uint8_t ack[4];
    uint8_t flags[2];
    uint8_t window_size[2];
    uint8_t checksum[2];
    uint8_t urgent_ptr[2];
};

struct TCP_packet {
    struct TCP_header hdr;
    uint8_t *payload;
    uint16_t payload_len;
};

std::optional<TCP_packet> parse_tcp(IPV4_packet *);