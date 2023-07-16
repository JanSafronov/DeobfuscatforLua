#include "static_chunk_analysis.hpp"
#include "vm_arch/control_flow_graph.hpp"
#include "optimizations/common.hpp"
#include "constant_decryption/constant_decryption_recongizer.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis {

	void static_chunk_analysis::populate_constant_owners() {
		for (auto& instruction : chunk->instructions) {
			if (!(instruction->is_ka || instruction->is_kb || instruction->is_kc))
				continue;

			std::unordered_set<std::uint16_t> set_targets(3ul); // to handle duplicates

			if (instruction->is_ka) {
				set_targets.emplace(instruction->a - 1);
				const auto& constant = chunk->constants.at(instruction->a - 1).get();
				constant->owners.emplace_back(std::cref(*instruction));
			}

			if (instruction->is_kb) {
				std::size_t constant_number = 0;
				if (instruction->b > 0) {
					constant_number = instruction->b - 1;
				}
				else {
					constant_number = instruction->bx - 1;
				}

				if (!set_targets.count(constant_number)) {
					set_targets.emplace(constant_number);
					const auto& constant = chunk->constants.at(constant_number).get();
					std::cout << "kst:" << constant->to_string() << std::endl;
					constant->owners.emplace_back(std::cref(*instruction));
				}
			}

			if (instruction->is_kc) {
				if (!set_targets.count(instruction->c - 1)) {
					const auto& constant = chunk->constants.at(instruction->c - 1).get();
					std::cout << "kst:" << constant->to_string() << std::endl;

					constant->owners.emplace_back(std::cref(*instruction));
				}
			}
		}
	}
	
	void static_chunk_analysis::remove_dead_constants() {
		// no STL because sucks
		std::unordered_map<std::size_t, std::size_t> shifted_constants(chunk->constants.size());

		// Fixed-Point Iteration For Shifting Constants And Removing Dead Ones
		auto constant_num = 0ul, num_dead_constants = 0ul;
		for (auto current_constant = 0ul; current_constant < chunk->constants.size(); ++current_constant, ++constant_num) {
			bool is_collectable = true;
			for (auto& owner : chunk->constants.at(current_constant)->owners) {
				if (!(owner.get().flags & instruction::instruction_mark_flag::to_collect)) {
					is_collectable = false;
					break;
				}
			}

			if (chunk->constants.at(current_constant)->owners.size() == 0 || is_collectable) {
				chunk->constants.erase(chunk->constants.begin() + current_constant);
				--current_constant;
				++num_dead_constants;
				continue;
			}

			shifted_constants.emplace(constant_num, current_constant);
		}
		std::cout << "deadkst:" << num_dead_constants << std::endl;

		// ... fix kst targets
		auto get_shifted_target = [&](std::size_t target) {
			if (auto result = shifted_constants.find(target - 1); result != shifted_constants.cend())
				return result->second + 1;

			return target;
		};

		for (auto& instruction : chunk->instructions) {
			if (instruction->is_ka) {
				instruction->a = get_shifted_target(instruction->a);
			}

			if (instruction->is_kb) {
				if (instruction->b > 0) {
					instruction->b = get_shifted_target(instruction->b);
				}
				else {
					instruction->bx = get_shifted_target(instruction->bx);
				}
			}

			if (instruction->is_kc) {
				instruction->c = get_shifted_target(instruction->c);
			}
		}
	}
	
	void static_chunk_analysis::propagate_instructions_pc() {
		std::size_t current_pc = 0;

		std::weak_ptr<vm_arch::basic_block> current_block{ cfg_result };
		while (!current_block.expired()) {
			auto result_block = current_block.lock();
			for (auto& instruction : result_block->instructions) {
				instruction_pc_mapping.emplace(&instruction.get(), current_pc);
				++current_pc;
			}

			current_block = result_block->next_block;
		}
	}

	void static_chunk_analysis::fix_branch_targets() {
		std::weak_ptr<vm_arch::basic_block> current_block{ cfg_result };
		while (!current_block.expired()) {
			auto result_block = current_block.lock();
			
			if (!result_block->instructions.empty()) {
				auto last_instruction = result_block->instructions.back();
				auto last_opcode = last_instruction.get().op;
				if (last_opcode == vm_arch::opcode::op_jmp || last_opcode == vm_arch::opcode::op_forprep || last_opcode == vm_arch::opcode::op_forloop) {
					auto target_instruction = result_block->target_block->instructions.at(0);
					if (auto pc_result = instruction_pc_mapping.find(&target_instruction.get()); pc_result != instruction_pc_mapping.end()) {
						last_instruction.get().sbx = pc_result->second;
					}
				}
			}

			current_block = result_block->next_block;
		}
	}
}