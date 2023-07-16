// rewrite this entire bs sadpepe

#include "constant_folding.hpp"
#include "ast/ir/abstract_visitor_pattern.hpp"
#include "ast/ir/node.hpp"

#include <iostream>
#include <type_traits>
#include <algorithm>

namespace deobf::ast::optimization_passes {
	struct folding_visitor final : ir::abstract_visitor_pattern {
		std::optional<std::shared_ptr<ir::expression::expression>> optimize(ir::expression::unary_expression* expression) { // will cause a shared_ptr duplicate?
			using operation_t = typename ir::expression::unary_expression::operation_t;

			// todo optimization visitor or any pattern so we dont have to make those kind of shit?
			// making a template for those stuff is useless and fucks up with ADL

			switch (expression->operation) {
				case operation_t::minus: {
					if (auto body_literal = expression->body->as<ir::expression::numeral_literal>())
						return std::make_shared<ir::expression::numeral_literal>(-body_literal->value);
				}
				case operation_t::len: {
					if (auto table = expression->body->as<ir::expression::table>()) {
						const auto has_initial_rehashing = std::any_of(table->entries.cbegin(), table->entries.cend(), [](const auto& pair) {
							return pair.first->is<ir::expression::array_index_literal>() && pair.second->is<ir::expression::nil_literal>();
						});

						// todo nil sequences

						if (!has_initial_rehashing) {
							return std::make_shared<ir::expression::numeral_literal>(static_cast<double>(table->array_size));
						}
					}
					else if (auto string_literal = std::dynamic_pointer_cast<ir::expression::string_literal>(expression->body)) {
						return std::make_shared<ir::expression::numeral_literal>(string_literal->value.length());
					}
					break;
				}
				case operation_t::not : {
					if (expression->body->is<ir::expression::nil_literal>()) {
						return std::make_shared<ir::expression::boolean_literal>(false);
					}
					else if (auto bool_literal = expression->body->as<ir::expression::boolean_literal>()) {
						return std::make_shared<ir::expression::boolean_literal>(!bool_literal->value);
					}
				}
			}

			return std::nullopt;
		}

		std::optional<std::shared_ptr<ir::expression::expression>> optimize(ir::expression::binary_expression* expression)  { // folds number expressions. (todo concat in future?)
			auto left = expression->left->as<ir::expression::numeral_literal>();
			auto right = expression->right->as<ir::expression::numeral_literal>();
			
			if (!(left && right)) {
				return std::nullopt;
			}

			using operation_t = typename ir::expression::binary_expression::operation_t;
			switch (expression->operation) {
				case operation_t::none:

				// basic artimethic, [+-*/^%]
				case operation_t::add : {
					return std::make_shared<ir::expression::numeral_literal>(left->value + right->value);
				}
				case operation_t::sub: {
					return std::make_shared<ir::expression::numeral_literal>(left->value - right->value);
				}
				case operation_t::div: {
					if (right->value == 0)
						return std::nullopt;

					return std::make_shared<ir::expression::numeral_literal>(left->value / right->value);
				}
				case operation_t::mul: {
					return std::make_shared<ir::expression::numeral_literal>(left->value * right->value);
				}
				case operation_t::pow: {
					return std::make_shared<ir::expression::numeral_literal>(std::pow(left->value, right->value));
				}
				case operation_t::mod: {
					if (right->value == 0)
						return std::nullopt;

					return std::make_shared<ir::expression::numeral_literal>(std::fmod(left->value, right->value));
				}
				
				// boolean expression (epsilon in lua is different?)
				case operation_t::gt: {
					return std::make_shared<ir::expression::boolean_literal>(left->value > right->value);
				}
				case operation_t::lt: {
					return std::make_shared<ir::expression::boolean_literal>(left->value < right->value);
				}
				case operation_t::ge: {
					return std::make_shared<ir::expression::boolean_literal>(left->value >= right->value);
				}
				case operation_t::le: {
					return std::make_shared<ir::expression::boolean_literal>(left->value <= right->value);
				}
				case operation_t::eq: {
					return std::make_shared<ir::expression::boolean_literal>(left->value == right->value);
				}
				case operation_t::neq: {
					return std::make_shared<ir::expression::boolean_literal>(left->value != right->value);
				}

				// logical (short-circurit evals)
				case operation_t::and: {
					return right;
				}
				case operation_t::or: {
					return left;
				}

				/*default:
					throw std::logic_error("encountered an unimpelemented type of binary_expression? (binary_expression optimizer).");*/
			}

			return std::nullopt;
		}
	};

	void constant_folding::optimize() {
		folding_visitor base;
		root->accept(&base);
	}
}