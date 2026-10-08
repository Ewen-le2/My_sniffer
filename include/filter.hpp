#pragma once

#include <queue>
#include <iostream>
#include "protocol.hpp"
#include "fcn_helper.hpp"

void execute_filter(const std::string& cmd, const std::deque<ETH_packet>& history);
