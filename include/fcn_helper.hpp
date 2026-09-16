#pragma once

#include <cstdint>
#include <format>
#include <iostream>

#include "header.hpp"

std::ostream& print_mac(std::ostream& os, const uint32_t hi, const uint16_t lo);

std::ostream& operator<<(std::ostream& os, const eth_header& h);
