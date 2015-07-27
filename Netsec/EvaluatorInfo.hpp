#pragma once

#include <time.h>

struct EvaluatorInfo {
	std::time_t timestamp;
	float probability;
	std::string type;
	std::string info;
};