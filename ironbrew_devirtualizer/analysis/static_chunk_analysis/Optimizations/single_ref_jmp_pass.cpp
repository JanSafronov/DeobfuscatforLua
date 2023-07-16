#include "single_ref_jmp_pass.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::single_ref_jmp_pass {
	
	std::size_t single_ref_jmp_pass::run(vm_arch::basic_block* first_block) {
		std::size_t num_optimizations = 0;

		auto current_block = first_block;
		while (current_block) {
			if (current_block->instructions.back().get().op == vm_arch::opcode::op_jmp) {
				auto target = current_block->target_block;
				if (target->instructions.size() == 1 && target->instructions.back().get().op == vm_arch::opcode::op_jmp) {
					auto next_target = target->next_block;
					current_block->target_block = target->target_block;
					++num_optimizations;
				}
			}

			current_block = current_block->next_block.get();
		}

		return num_optimizations;
	}
}
