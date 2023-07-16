#include "cmp_spam_pass.hpp"
#include <iostream>
#include <queue>

#include "vm_arch/auxiliaries.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::cmp_spam_pass {

	std::weak_ptr<vm_arch::basic_block> identify_final_target_successor(vm_arch::instruction& last_instruction, std::weak_ptr<vm_arch::basic_block> start) {
		auto current_block{ start };

		auto verify_block_sucessor = [&](vm_arch::basic_block* result) -> vm_arch::block_iterate_direction {
			auto first_instruction = result->instructions.front();

			if (auto instr_op = first_instruction.get().op; instr_op == vm_arch::opcode::op_jmp) {
				return vm_arch::block_iterate_direction::target;
			}
			else if (instr_op == last_instruction.op && first_instruction.get().a == last_instruction.a) {
				return vm_arch::block_iterate_direction::next;
			}
			else {
				auto normalized_opcode = aux::get_normalized_opcode(instr_op);
				if (normalized_opcode == vm_arch::opcode::op_ge || normalized_opcode == vm_arch::opcode::op_lt) {
					return vm_arch::block_iterate_direction::target;
				}
			}

			return { };
		};

		while (!current_block.expired()) {
			auto current_block_result = current_block.lock();

			// too lazy to optimize this false-generated shit on eq mutate and its probably redudrant
			{
				bool do_double_skip = false;

				auto normalized_opcode = aux::get_normalized_opcode(current_block_result->instructions.front().get().op);

				if (normalized_opcode == vm_arch::opcode::op_ge || normalized_opcode == vm_arch::opcode::op_lt) {
					auto target = current_block_result->target_block;
					if (target->instructions.size() == 1) {
						auto normalized_opcode_2 = aux::get_normalized_opcode(target->instructions.front().get().op);
						if (normalized_opcode_2 == vm_arch::opcode::op_gt || normalized_opcode_2 == vm_arch::opcode::op_le) {
							do_double_skip = true;
						}
					}
				}

				if (do_double_skip) {
					current_block = current_block_result->target_block->next_block; //: current_block_result->target_block->target_block;
					continue;
				}
			}

			switch (verify_block_sucessor(current_block_result.get())) {
				case vm_arch::block_iterate_direction::next: {
					current_block = current_block_result->next_block;
					break;
				}
				case vm_arch::block_iterate_direction::target: {
					if (verify_block_sucessor(current_block_result->target_block.get()) == vm_arch::block_iterate_direction::none) {
						current_block_result->instructions.front().get().print();
						goto return_result;
					}

					current_block = current_block_result->target_block;
					break;
				}
				default:
					goto return_result;
			}
		}

		//current_block.lock()->instructions.front().get().print();
		return_result:
		return current_block;
	}
}
