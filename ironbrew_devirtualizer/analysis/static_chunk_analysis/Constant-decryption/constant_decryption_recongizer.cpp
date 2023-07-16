#pragma once
#include "constant_decryption_recongizer.hpp"

#include <stack>
#include <queue>
#include <bitset>

// TODO REWRITE

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::constant_decryption {
	using namespace vm_arch;

	inline std::string constant_decryption_recongizer::decrypt_string(const std::string& parameter, const std::string& round_xor_key) {
		std::string decrypted_result;
		for (auto i = 0ul; i < parameter.size(); ++i) {
			const auto buffer_index = i % round_xor_key.size();
			decrypted_result += parameter[i] ^ round_xor_key[buffer_index];
		}
		return decrypted_result;
	}

	// TODO recongizer and solver classes?
	void constant_decryption_recongizer::run() {
		auto current_block{ analyzer.cfg_result };

		// Only applies where call stack has only register references, and CLOSURE does not apply as OP_CALL's reference
		static auto resolve_register_call_stack = [](std::vector<control_flow_graph::instruction_t>& instructions, std::size_t call_offset) -> auto {
			std::stack<std::int16_t> register_call_stack; // preserve for 256 registers

			std::uint8_t call_stack_begin = 0;

			for (; call_offset > 0; --call_offset) {
				auto& current_instruction = instructions.at(call_offset).get();

				if (!call_stack_begin) {
					call_stack_begin = current_instruction.a;
					continue;
				}

				
				if (current_instruction.op == vm_arch::opcode::op_move) {
					if (current_instruction.a == call_stack_begin) {
						break;
					}
					else {
						register_call_stack.push(instructions.at(call_offset).get().b);
					}
				}
			}

			return register_call_stack;
		};

		while (current_block->next_block != nullptr) {
			const auto max_instructions = current_block->instructions.size();

			// recongize string decryption (condition : ENCRYPTION_BLOCK_ENTRY dom CURRENT_BLOCK (dominator))

			if (max_instructions > 2) {
				const auto last_instruction = current_block->instructions.at(max_instructions - 2);

				// goto the terminator instruction for inlined function xor (Settop)
				if (last_instruction.get().op == opcode::op_settop && last_instruction.get().a == 14) {
					// parameter A is maxstacksize of xor function this case (stack is deallocated on next basic block).
					// only possible on string encryption, shouldInline enables string encryption from doing such stuff.

					std::uint16_t parameter_reference = 0, circular_buffer_reference = 0;

					std::weak_ptr<vm_arch::basic_block> final_block(current_block->next_block->next_block);

					auto current_predecessor = current_block->block_predecessors.back();
					while (!current_predecessor.expired()) {
						auto predecessor_result = current_predecessor.lock();

						// round key loop follows the pattern : sub mod add
						static const auto xor_loop_pattern = { opcode::op_sub2, opcode::op_mod2, opcode::op_add2 };

						if (predecessor_result->instructions.at(0).get().op == opcode::op_sub2 
							&& predecessor_result->instructions.at(1).get().op == opcode::op_mod2 
							&& predecessor_result->instructions.at(2).get().op == opcode::op_add2) {
							// first op_call third stack slot value is the function parameter (op_move called at start to another register and loadk on original)
							// second op_call is linked to xor circular buffer (length = original message size)

							constexpr std::size_t xor_call_1_offset = 0x9; // call p0
							constexpr std::size_t xor_call_2_offset = 0xF; // call p1

							//auto function_reference_1 = predecessor_result->instructions.at(xor_call_1_offset).get().a;
							//auto function_reference_2 = predecessor_result->instructions.at(xor_call_2_offset).get().a;

							auto register_call_stack_1 = resolve_register_call_stack(predecessor_result->instructions, xor_call_1_offset);
							auto register_call_stack_2 = resolve_register_call_stack(predecessor_result->instructions, xor_call_2_offset);

							// populate operands
							parameter_reference = register_call_stack_1.top();
							circular_buffer_reference = register_call_stack_2.top();

							std::cout << static_cast<int>(parameter_reference) << std::endl;
							std::cout << static_cast<int>(circular_buffer_reference) << std::endl;

							break;
						}
						
						if (predecessor_result->block_predecessors.empty())
							break;

						current_predecessor = predecessor_result->block_predecessors.back();
					}

					if (!(parameter_reference || circular_buffer_reference))
						return;


					std::pair<std::int32_t, std::string> symmetric_circular_buffer, encrypted_parameter;
				}
			}

			current_block = current_block->next_block;
		}

		// final pass
		//analyzer.eliminate_duplicated_constants();

		std::cout << "Donejob\n";
		
	}
}