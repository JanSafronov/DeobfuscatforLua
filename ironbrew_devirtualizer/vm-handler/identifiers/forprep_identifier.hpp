#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct forprep_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			auto if_stat = path->find_first_of<ir::statement::if_statement>();
			if (if_stat.has_value()) {
				if (auto condition = if_stat->get().condition->as<ir::expression::binary_expression>()) {
					if (condition->operation == ir::expression::binary_expression::operation_t::gt && condition->right->to_string() == "0") {
						if (auto symbol = path->find_symbol(condition->left->to_string())) {
							if (symbol->symbol_value->to_string() == "stack[( instruction_opcode_a + 2 )]") {

								return vm_arch::opcode::op_forprep;
							}
						}
					}
				}
			}
			
			return __super::handle(path);
		}
	};
}