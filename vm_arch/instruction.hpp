#pragma once
#include <cstdint>
#include "opcode.hpp"
#include <iostream>
#include <unordered_set>
#include <bitset>

namespace deobf::vm_arch {
	struct instruction final {
		vm_arch::opcode op{ };
		vm_arch::instruction_type type{ }; // couldve made a mapping but lazy

		enum instruction_mark_flag {
			none = 1 << 0,
			to_collect = 1 << 1
		};

		std::size_t flags = instruction_mark_flag::none;

		// on mutated ops such eq/lt/le
		// couldve encoded all isK flags together but too lazy to change stuff so im going to keep this like that
		bool is_ka = false;
		bool is_kb = false;
		bool is_kc = false;
		
		std::size_t line_defined;
		double virtual_opcode;
		std::int16_t a;

		// optional parameters
		std::int16_t b{ };
		std::int16_t c{ };
		std::int32_t bx{ };
		std::int32_t sbx{ };
	};
}