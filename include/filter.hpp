#pragma once

#include <queue>
#include <iostream>
#include "protocol.hpp"
#include "fcn_helper.hpp"
#include <optional>
#include <array>
#include <algorithm>

enum class Direction {
    Any,
    In,   // Destination
    Out   // Source
};

void execute_filter(const std::string& cmd, const std::deque<ETH_packet>& history);
std::optional<Direction> parse_direction(const std::string& text);
bool filter_proto(const ETH_packet&, const std::string&);
bool filter_port(const ETH_packet&, uint16_t, Direction);
std::optional<std::array<uint8_t, 4>> parse_filter_ip(const std::string&);
bool filter_ip(const ETH_packet&, const std::array<uint8_t, 4>&, Direction);
std::optional<std::array<uint8_t, 6>> parse_filter_mac(const std::string&);
bool filter_mac(const ETH_packet&, const std::array<uint8_t, 6>&, Direction);
bool matches_port(uint16_t src, uint16_t dst, uint16_t wanted, Direction direction);
bool matches_address(const uint8_t* src, const uint8_t* dst, const uint8_t* wanted, std::size_t size, Direction direction);
void print_all(const ETH_packet& eth);
uint16_t read_port(const uint8_t port[2]);