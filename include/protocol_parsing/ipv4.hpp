#pragma once

#include "protocol.hpp"
#include <cstdint>
#include <optional>

#define Proto_TCP 0x06
#define Proto_UDP 0x11

struct IPV4_header {
    uint8_t Version;
    uint8_t Service;
    uint8_t Lenght[2];
    uint8_t Identification[2];
    uint8_t Flags[2];
    uint8_t TTL;
    uint8_t Protocol; //-> TCP/UDP/ETC...
    uint8_t Checksum[2];
    uint8_t SourceIP[4];
    uint8_t DestinationIP[4];
};

struct IPV4_packet {
    struct IPV4_header hdr;
    uint8_t *payload;
    uint16_t payload_len;
};

std::optional<IPV4_packet> parse_ipv4(ETH_packet *);