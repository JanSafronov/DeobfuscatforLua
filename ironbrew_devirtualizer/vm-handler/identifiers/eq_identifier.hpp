#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct eq_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() == 1) {
				if (auto if_stat = path->body.at(0)->as<ir::statement::if_statement>()) {
					const auto expression_string = if_stat->condition->to_string();

					// eq
					if (expression_string == "( instruction_opcode_a == instruction_opcode_c )") {
						return vm_arch::opcode::op_eq3;
					}

					// ne

					if (expression_string == "( instruction_opcode_a ~= instruction_opcode_c )") {
						return vm_arch::opcode::op_ne3;
					}
				}
			}

			return __super::handle(path);
		}
	};
}