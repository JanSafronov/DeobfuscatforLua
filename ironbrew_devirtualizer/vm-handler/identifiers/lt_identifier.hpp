#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct lt_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() == 1) {
				if (auto if_stat = path->body.at(0)->as<ir::statement::if_statement>()) {
					const auto expression_string = if_stat->condition->to_string();
					if (if_stat->body->body.size() != 1) {
						return __super::handle(path);
					}

					bool is_ge = (if_stat->body->body.at(0)->to_string() != "instruction_pointer = instruction_opcode_b");
				}
			}

			return __super::handle(path);
		}
	};
}