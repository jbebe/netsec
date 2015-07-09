#pragma once

#include <atomic>

static std::atomic<bool> RUN;

std::atomic<int> performance_counter{0};
