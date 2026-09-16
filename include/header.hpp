#ifndef HEADER_HPP_
#define HEADER_HPP_

#include <cstdint>

/*
    802.3 Ethernet packet and frame structure

    MAC destination:        6 bytes
    MAC source:             6 bytes

    802.1Q tag (optional):  4 bytes

    Ethertype (Ethernet II)
    or length (IEEE 802.3): 2 bytes

    Payload:                42-1500 bytes (mtu)
*/

/*
struct header {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t eth_type;
    uint8_t paylod[]; // 42-1500 bytes; Payload can be 42 octets if an 802.1Q tag is present. Minimum is 46 octets without.
};
*/

#define ETH_T_IPV4  0x0800
#define ETH_T_IPV6  0x86DD
#define ETH_T_ARP   0x0806

struct eth_header {
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t ethertype;
};

struct eth_packet {
    struct eth_header *hdr;
    uint8_t *payload;
    uint16_t payload_len;
};

#endif /* HEADER_HPP_ */

