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

		// final pass
		//analyzer.eliminate_duplicated_constants();

		std::cout << "Donejob\n";
		
	}
}