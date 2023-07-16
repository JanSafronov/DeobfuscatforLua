#include "encryptedkst_propagation_pass.hpp"
#include "vm_arch/auxiliaries.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::encryptedkst_propagation_pass {

	std::size_t encryptedkst_propagation_pass::run(vm_arch::basic_block* first_block) {
		std::size_t num_optimizations = 0;

		auto current_block = first_block;
		while (current_block->next_block) {
			auto& last_instruction = current_block->instructions.back().get();
			if (last_instruction.op == vm_arch::opcode::op_decryptedkstflag) {
				const auto register_used = last_instruction.a;
				const auto constant_reference = last_instruction.bx;


				// todo propagate basicblocks?

				++num_optimizations;
			}
			

			current_block = current_block->next_block.get();
		}

		return num_optimizations;
	}
}
