#include "instruction_translator.hpp"

namespace deobf::ironbrew_devirtualizer::vanilla_lifter {
	std::unique_ptr<vm_arch::vanilla_instruction> instruction_translator::convert_instruction(vm_arch::instruction* original_instruction, std::uint32_t pc) {
		auto new_instruction = std::make_unique<vm_arch::vanilla_instruction>();
		
		//if (original_instruction->op == vm_arch::opcode::op_invalid)
		//	throw std::runtime_error("invalid opcode");

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
			case vm_arch::instruction_type::abx: { // SET ARG BX
				new_instruction->type = vm_arch::vanilla_instruction_type::abx;
				new_instruction->bx = original_instruction->bx;
				break;
			}
			case vm_arch::instruction_type::asbx: { // SET ARG SBX
				new_instruction->type = vm_arch::vanilla_instruction_type::asbx;
				new_instruction->sbx = original_instruction->sbx;
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
			case vm_arch::opcode::op_setlist2: {
				// todo (rare case altho we might hit block 512 on big tables)
				new_instruction->c = 0;
				break;
			}
			case vm_arch::opcode::op_closure: { // upvalues are resolved by operand C at runtime in ironbrew, lua Vm already resolves it, null out C
				new_instruction->type = vm_arch::vanilla_instruction_type::abx;

				new_instruction->sbx = 0;
				new_instruction->c = 0;
				new_instruction->bx = original_instruction->bx;

				break;
			}
			case vm_arch::opcode::op_tforloop: {
				new_instruction->type = vm_arch::vanilla_instruction_type::ac;

				new_instruction->b = 0;
				break;
			}
			case vm_arch::opcode::op_settable1: {
				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_settable2: {
				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_settable3: {
				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}
			// arthimethic
			case vm_arch::opcode::op_add1: {
				new_instruction->b += 0xff;
				break;
			}
		}

		return std::move(new_instruction);
	}
}