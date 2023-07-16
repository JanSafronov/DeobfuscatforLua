#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"
#include "opcode_handler_computer.hpp"
#include "./ironbrew_devirtualizer/vm_handler_identifiers/identifier_base.hpp"

#include <future>
#include <algorithm>
#include <numeric>

namespace deobf::ironbrew_devirtualizer::symbolic_execution::vm {
	using namespace ast;

	//decltype(loop_unrolled_bst::callback_functor) loop_unrolled_bst::callback_handler;

	bool opcode_handler_computer::accept(ir::expression::binary_expression* expression) {
		if (back_track.empty())
			return false;

		if (expression->left->is<ir::expression::variable>())
			if (expression->left->to_string() != "virtual_opcode")
				return false;

		if (expression->right->is<ir::expression::numeral_literal>()) {
			const auto right_value = static_cast<const ir::expression::numeral_literal*>(expression->right.get());

			using operation_t = typename ir::expression::binary_expression::operation_t;

			const auto virtual_opcode = back_track.front().get().virtual_opcode;

			switch (expression->operation) { // concerete execute for bound
			case operation_t::le:
				return virtual_opcode <= right_value->value; // this is a divide and conquer route, continue to next if statements
			case operation_t::gt:
				return virtual_opcode > right_value->value;
			case operation_t::eq:
				return virtual_opcode == right_value->value;
			}
		}

		return false;
	}

	bool opcode_handler_computer::accept(ir::statement::block* body) {
		if (body->body.size() == 1 && body->body.at(0)->is<ir::statement::if_statement>()) {
			auto if_statement = static_cast<ir::statement::if_statement*>(body->body.at(0).get());
			if (if_statement->condition->is<ir::expression::binary_expression>()) {
				const auto statment_lhs = static_cast<ir::expression::binary_expression*>(if_statement->condition.get())->left;

				if (statment_lhs->is<ir::expression::variable>() && statment_lhs->to_string() == "virtual_opcode") {
					if_statement->accept(this);
					return false;
				}
			}
		}

		if (back_track.empty()) {
			return false;
	}

}
