#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct sub_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() == 1 && path->body.at(0)->is<ir::statement::variable_assign>()) {
				const auto assign_string = path->body.at(0)->to_string();
				if (assign_string == "stack[instruction_opcode_a] = ( instruction_opcode_b - instruction_opcode_c )") {
					return vm_arch::opcode::op_sub3;
				}
				else if (assign_string == "stack[instruction_opcode_a] = ( stack[instruction_opcode_b] - instruction_opcode_c )") {
					return vm_arch::opcode::op_sub2;
				}
				else if (assign_string == "stack[instruction_opcode_a] = ( instruction_opcode_b - stack[instruction_opcode_c] )") {
					return vm_arch::opcode::op_sub1;
				}
				else if (assign_string == "stack[instruction_opcode_a] = ( stack[instruction_opcode_b] - stack[instruction_opcode_c] )") {
					return vm_arch::opcode::op_sub;
				}
			}

			return __super::handle(path);
		}
	};
}