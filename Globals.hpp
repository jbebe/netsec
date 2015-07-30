#pragma once

#include <thread>
#include <atomic>

constexpr size_t CONSUMER_BUFFER_SIZE = 16;
constexpr size_t STATS_ENTRY_SIZE = 128;

const size_t CORE_NUM = std::max<size_t>(std::thread::hardware_concurrency(), 1);

std::atomic_bool RUN_PRODUCER{true};
std::atomic_bool RUN_CONSUMER{true};