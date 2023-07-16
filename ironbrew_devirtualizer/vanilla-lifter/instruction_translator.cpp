#include "instruction_translator.hpp"

namespace deobf::ironbrew_devirtualizer::vanilla_lifter {
	std::unique_ptr<vm_arch::vanilla_instruction> instruction_translator::convert_instruction(vm_arch::instruction* original_instruction, std::uint32_t pc) {
		auto new_instruction = std::make_unique<vm_arch::vanilla_instruction>();


		auto new_opcode = read_single_data_opcode_to_vanilla(original_instruction->op);

		new_instruction->line_defined = original_instruction->line_defined;
		new_instruction->opcode = new_opcode;
		new_instruction->a = original_instruction->a;
		new_instruction->type = vm_arch::vanilla_instruction_type::a;

		switch (original_instruction->type) {
			case vm_arch::instruction_type::abc: { // SET ARG B & SET ARG C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;
				new_instruction->b = original_instruction->b;
				new_instruction->c = original_instruction->c;
				break;
			}
			case vm_arch::instruction_type::asbxc: { // AC & SBX, SET ARG SBX & SET ARG C (WILL BE DISCARDED LATER)
				original_instruction->print();
				new_instruction->sbx = original_instruction->sbx;
				new_instruction->c = original_instruction->c;
				break;
			}
			default:
				throw std::runtime_error("[ironbrew_deobfuscator/final_step/vanilla_lifter/instruction_translator]: invalid instruction type");
		}

		// transpile instructions (could be done shorter and way better, but i prefer like that)

		// TODO : new_instruction->(b || c) += 0xff; could be done efficencfly with encode_kst_index(idx + 1)

		// kst_start = (1 << (MAX_B - 1)) - 1 = 256 - 1 = 0xff

		switch (original_instruction->op) {
			case vm_arch::opcode::op_setlist: {
				new_instruction->b -= original_instruction->a;
				break;
			}
		}

		return std::move(new_instruction);
	}
}