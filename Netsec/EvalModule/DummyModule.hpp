#pragma once

#include "../Core/Evaluator.hpp"

void dummy_module(const std::vector<Evaluator::plugin_pair> &plugin_pairs){
	for (auto &pair : plugin_pairs){
		std::tm *ptm = std::localtime(&(pair.first.timestamp));
		char buffer[32];
		std::strftime(buffer, 32, "%Y.%m.%d %H:%M:%S", ptm); 
		dbg_printf("\033[2;38H\033[JDate: %s", buffer);
		dbg_printf("\033[3;38H\033[JType: %s", pair.first.type.c_str());
		dbg_printf("\033[4;38H\033[JProbability: %f", pair.first.probability);
		dbg_printf("\033[5;38H\033[JInfo: %s", pair.first.info.c_str());
		return;
	}
}