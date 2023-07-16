#pragma once
#include <unordered_map>
#include "vm_arch/opcode.hpp"

namespace deobf::ironbrew_devirtualizer::vanilla_lifter {
	// opcode mapping
	static const std::unordered_map<vm_arch::opcode, vm_arch::vanilla_opcode> vanilla_opcode_mapping{
		{ vm_arch::opcode::op_move, vm_arch::vanilla_opcode::op_move },

		{ vm_arch::opcode::op_loadk, vm_arch::vanilla_opcode::op_loadk },

		{ vm_arch::opcode::op_loadbool, vm_arch::vanilla_opcode::op_loadbool },
		{ vm_arch::opcode::op_loadbool1, vm_arch::vanilla_opcode::op_loadbool },

		{ vm_arch::opcode::op_loadnil, vm_arch::vanilla_opcode::op_loadnil },

		{ vm_arch::opcode::op_getupval, vm_arch::vanilla_opcode::op_getupval },

		{ vm_arch::opcode::op_getglobal, vm_arch::vanilla_opcode::op_getglobal },

		{ vm_arch::opcode::op_gettable, vm_arch::vanilla_opcode::op_gettable },
		{ vm_arch::opcode::op_gettable1, vm_arch::vanilla_opcode::op_gettable },

		{ vm_arch::opcode::op_setglobal, vm_arch::vanilla_opcode::op_setglobal },

		{ vm_arch::opcode::op_setupval, vm_arch::vanilla_opcode::op_setupval },

		{ vm_arch::opcode::op_settable, vm_arch::vanilla_opcode::op_settable },
		{ vm_arch::opcode::op_settable1, vm_arch::vanilla_opcode::op_settable },
		{ vm_arch::opcode::op_settable2, vm_arch::vanilla_opcode::op_settable },
		{ vm_arch::opcode::op_settable3, vm_arch::vanilla_opcode::op_settable },

		{ vm_arch::opcode::op_newtable, vm_arch::vanilla_opcode::op_newtable },

		{ vm_arch::opcode::op_self, vm_arch::vanilla_opcode::op_self },
		{ vm_arch::opcode::op_self1, vm_arch::vanilla_opcode::op_self },

		{ vm_arch::opcode::op_add, vm_arch::vanilla_opcode::op_add },
		{ vm_arch::opcode::op_add1, vm_arch::vanilla_opcode::op_add },
		{ vm_arch::opcode::op_add2, vm_arch::vanilla_opcode::op_add },
		{ vm_arch::opcode::op_add3, vm_arch::vanilla_opcode::op_add },

		{ vm_arch::opcode::op_sub, vm_arch::vanilla_opcode::op_sub },
		{ vm_arch::opcode::op_sub1, vm_arch::vanilla_opcode::op_sub },
		{ vm_arch::opcode::op_sub2, vm_arch::vanilla_opcode::op_sub },
		{ vm_arch::opcode::op_sub3, vm_arch::vanilla_opcode::op_sub },

	};
};