#pragma once

#include <time.h>

struct EvaluatorInfo {
	std::time_t timestamp = std::time_t{};
	float probability     = float{};
	std::string type      = std::string{};
	std::string info      = std::string{};
};