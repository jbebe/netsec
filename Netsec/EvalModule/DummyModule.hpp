#pragma once

#include "../Core/Evaluator.hpp"
#include "../ParsedPacketElem.hpp"
#include "../EvaluatorInfo.hpp"

void dummy_module(const std::vector<ParsedPacketElem> &parse_vec, const std::vector<Evaluator::PluginPair> &eval_vec){
	/*
	std::tm *ptm = std::localtime(&(eval_info.timestamp));
	char buffer[32];
	std::strftime(buffer, 32, "%Y.%m.%d %H:%M:%S", ptm);
	dbg_printf("IP: %s", parse_info.)
	dbg_printf("\033[2;38H\033[JDate: %s", buffer);
	dbg_printf("\033[3;38H\033[JType: %s", eval_info.type.c_str());
	dbg_printf("\033[4;38H\033[JProbability: %f", eval_info.probability);
	dbg_printf("\033[5;38H\033[JInfo: %s", eval_info.info.c_str());
	*/
}