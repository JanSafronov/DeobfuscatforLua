#include "regkst_propagation_pass.hpp"
#include "vm_arch/auxiliaries.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::regkst_propagation_pass {
	std::size_t regkst_propagation_pass::run(vm_arch::basic_block* first_block) {
		std::size_t num_optimizations = 0;

		auto current_block = first_block;
		while (current_block->next_block) {
			auto last_instruction = current_block->instructions.back();

			const auto last_opcode = last_instruction.get().op;

			if (current_block->instructions.size() < 3)
				goto continue_execution;
				
			if (last_opcode == vm_arch::opcode::op_le ||
				last_opcode == vm_arch::opcode::op_gt ||
				last_opcode == vm_arch::opcode::op_lt ||
				last_opcode == vm_arch::opcode::op_ge ||
				last_opcode == vm_arch::opcode::op_ne ||
			last_opcode == vm_arch::opcode::op_eq) {
				
				constexpr std::uint16_t reg_target_a = 250;
				constexpr std::uint16_t reg_target_c = 251;

				const auto reg_a = last_instruction.get().a;
				const auto reg_c = last_instruction.get().c;

				if (reg_a != reg_target_a && reg_c != reg_target_c)
					goto continue_execution;

				const auto reg_a_reference = current_block->instructions.at(current_block->instructions.size() - 3);
				const auto reg_c_reference = current_block->instructions.at(current_block->instructions.size() - 2);
				reg_a_reference.get().print();
				reg_c_reference.get().print();

				const auto is_ka = (reg_a_reference.get().op == vm_arch::opcode::op_loadk);
				const auto is_kc = (reg_c_reference.get().op == vm_arch::opcode::op_loadk);

				const auto original_reg_a = is_ka ? reg_a_reference.get().bx : reg_a_reference.get().b;
				const auto original_reg_c = is_kc ? reg_c_reference.get().bx : reg_c_reference.get().b;

				std::cout << (reg_a_reference.get().op != vm_arch::opcode::op_move) << std::endl;

				if (reg_a_reference.get().op != vm_arch::opcode::op_move && !is_ka)
					goto continue_execution;

				if (reg_c_reference.get().op != vm_arch::opcode::op_move && !is_kc)
					goto continue_execution;
			}
			// handle relational operators

			else if (last_opcode == vm_arch::opcode::op_test ||
				last_opcode == vm_arch::opcode::op_test1 ||
				last_opcode == vm_arch::opcode::op_testset ||
		}

		return num_optimizations;
	}
}
