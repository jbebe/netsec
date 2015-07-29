#pragma once

#include <thread>

constexpr size_t CONSUMER_BUFFER_SIZE = 16;
constexpr size_t STATS_ENTRY_SIZE = 128;

const size_t CORE_NUM = std::max<size_t>(std::thread::hardware_concurrency(), 1);
