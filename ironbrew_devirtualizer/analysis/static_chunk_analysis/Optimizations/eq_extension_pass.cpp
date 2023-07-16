#include "eq_extension_pass.hpp"
#include "vm_arch/auxiliaries.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::eq_extension_pass {

	static std::unordered_map<vm_arch::opcode, std::tuple<vm_arch::opcode, vm_arch::opcode>> last_to_first_mapping {
		// EQ A == 1 (should generate NE)
		{ vm_arch::opcode::op_ge, { vm_arch::opcode::op_le, vm_arch::opcode::op_ne} },
		{ vm_arch::opcode::op_ge1, { vm_arch::opcode::op_le1, vm_arch::opcode::op_ne1 } },
		{ vm_arch::opcode::op_ge2, { vm_arch::opcode::op_le2, vm_arch::opcode::op_ne2 } },
		{ vm_arch::opcode::op_ge3, { vm_arch::opcode::op_le3, vm_arch::opcode::op_ne3 } },

		// EQ A == 0
		{ vm_arch::opcode::op_lt, { vm_arch::opcode::op_gt, vm_arch::opcode::op_eq } },
		{ vm_arch::opcode::op_lt1, { vm_arch::opcode::op_gt1, vm_arch::opcode::op_eq1 } },
		{ vm_arch::opcode::op_lt2, { vm_arch::opcode::op_gt2, vm_arch::opcode::op_eq2 } },
		{ vm_arch::opcode::op_lt3, { vm_arch::opcode::op_gt3, vm_arch::opcode::op_eq3 } },
	};

	std::size_t eq_extension_pass::run(vm_arch::basic_block* first_block) {
		std::size_t num_optimizations = 0;

		auto current_block = first_block;
		while (current_block->next_block) {
			auto last_instruction = current_block->instructions.back();
			
			if (auto new_opcode = last_to_first_mapping.find(last_instruction.get().op); new_opcode != last_to_first_mapping.cend()) { // keep in mind ge's type is asbxc
				auto r_a = last_instruction.get().a;
				auto target_leader = last_instruction.get().sbx;
				auto r_c = last_instruction.get().c;

				// iterate successors instead todo?
				auto next_block = current_block->target_block;
			}
			continue_search:
			current_block = current_block->next_block.get();
		}

		return num_optimizations;
	}
}
