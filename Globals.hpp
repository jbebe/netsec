#pragma once

#include <atomic>

static std::atomic<bool> RUN;

std::atomic<unsigned long> performance_counter{0};
