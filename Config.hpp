#pragma once

#include <atomic>
#include <thread>

namespace NETSEC {

constexpr bool DEBUG = true;
constexpr int BUFF_CAPACITY = 8;
const int CORE_COUNT = std::thread::hardware_concurrency();
std::atomic<bool> STOP{false};

}