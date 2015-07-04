#pragma once

namespace NETSEC {

constexpr int CYCBUFF_CAPACITY = 1024;

const uint32_t CORE_COUNT = std::thread::hardware_concurrency();

}