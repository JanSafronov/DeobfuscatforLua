#pragma once

#include <memory>
#include <unordered_map>

#include "vm_arch/vanilla_instruction.hpp"
#include "vm_arch/instruction.hpp"
#include "single_opcode_mapping.hpp"

namespace deobf::ironbrew_devirtualizer::vanilla_lifter {
	class instruction_translator {
		[[deprecated]] static inline vm_arch::instruction_type get_op_mode(vm_arch::instruction_type old) {
			switch (old) {
				case vm_arch::instruction_type::asbxc: {
					return vm_arch::instruction_type::abc; // usualy AC too but uh
				}
				default:
					break;
			}

			return old;
		}

	public:
		static std::unique_ptr<vm_arch::vanilla_instruction> convert_instruction(vm_arch::instruction*, std::uint32_t);
	};
}