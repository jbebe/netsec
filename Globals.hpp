#pragma once

#include <thread>

size_t STATS_ENTRY_SIZE = 100;

size_t CORE_NUM = std::max<size_t>(std::thread::hardware_concurrency(), 1);