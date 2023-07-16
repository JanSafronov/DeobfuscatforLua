#pragma once
#include "decryption_block_transformer.hpp"

#include <queue>

// todo optimize this code
namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::constant_decryption {
	bool decryption_block_transformer::optimize() {
		// todo fix branch targets (might be unnecessary?)
		// todo handle debug lines?
		// todo fix duplicates

		// set encrypted constant to decrypted constant ref in proto

		auto begin_block = begin.lock();
		if (!begin_block)
			return false;


		auto final_block = end.lock();
		if (!final_block)
			return false;


		auto& constant_result = analyzer.chunk->constants.at(parameter_reference);
		constant_result->value = decrypted_constant;
		//constant_result->flags |= constant::constant_mark_flag::decrypted_constant;


		// purify begin_block & flag
		for (;; begin_block->instructions.pop_back()) {
			auto current_instruction = begin_block->instructions.back();

			// todo replace loadk with real load constant instruction (optimization pass)
			if (current_instruction.get().op == vm_arch::opcode::op_loadk && current_instruction.get().bx - 1 == parameter_reference) {
				const auto move_instruction = final_block->instructions.at(0);
				if (move_instruction.get().op != vm_arch::opcode::op_move)
					return false;

				current_instruction.get().a = move_instruction.get().a; // swap register values

				//current_instruction.get().print();

				break;
			}

			current_instruction.get().flags |= instruction::instruction_mark_flag::to_collect;
		}

		return true;
	}
}