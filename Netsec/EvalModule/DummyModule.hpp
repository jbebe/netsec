#pragma once

#include "../Core/Evaluator.hpp"
#include "../ParsedPacketElem.hpp"
#include "../EvaluatorInfo.hpp"

void dummy_module(const std::vector<ParsedPacketElem> &parse_vec, const std::vector<Evaluator::PluginPair> &eval_vec){
	for (auto &eval : eval_vec){
		dbg_printf("[date: %s][type: %s][info: %s][probability: %s]\n", 
			eval.data.timestampStr().c_str(),
			eval.data.type.c_str(),
			eval.data.info.c_str(),
			eval.data.probabilityStr().c_str()
		);
	}
}