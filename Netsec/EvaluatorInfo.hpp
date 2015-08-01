#pragma once

#include <time.h>

struct EvaluatorInfo {
	
	std::time_t timestamp = std::time_t{};
	float probability     = float{};
	std::string type      = std::string{};
	std::string info      = std::string{};
	
	std::string timestampStr() const {
		std::tm *tm_ptr = std::localtime(&timestamp);
		char buffer[32];
		std::strftime(buffer, 32, "%Y.%m.%d %H:%M:%S", tm_ptr);
		return std::string{buffer};
	}
	
	std::string probabilityStr() const {
		char buffer[5];
		snprintf(buffer, 5, "%3.0f%%", probability*100.0f);
		return std::string{buffer};
	}
};