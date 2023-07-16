#pragma once

#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"
#include "symbolic_deserializer.hpp"
#include "deserializer_context.hpp"

#include <stack>


namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
	using namespace ast;
		
	using namespace deserializer_enums;

	struct deserializer_visitor final : ir::abstract_visitor_pattern { // process chunks via symbolic execution
		explicit deserializer_visitor(symoblic_deserializer& const object) :
			object(object)
		{ }

		const constant_type determine_constant_index(const ir::statement::block* path) { 
			constant_type type{ };
			
			if (auto assign = path->find_first_of<ir::statement::variable_assign>(); assign.has_value()) {
				const auto& assign_stat = assign->get();
				if (assign_stat.variables.size() == 1 && assign_stat.expressions.size() == 1) {
					const auto expression_string = assign_stat.expressions.at(0)->to_string();
					if (expression_string == "( get_8_bits() ~= 0 )")
						type = constant_type::boolean;
					else if (expression_string == "get_float()")
						type = constant_type::number;
					else if (expression_string == "get_string()")
						type = constant_type::string;
				}
			}

			return type;
		}

		//inline void populate_constant_order(ir::statement::block*& statement) {

		// todo pair might be slower?
		inline void populate_constant_order(const std::pair<const ir::expression::binary_expression*, const ir::statement::block*>& condition_pair) {
			const auto result = determine_constant_index(condition_pair.second);
			if (auto numeral_right = condition_pair.first->right->as<ir::expression::numeral_literal>()) {
				object.deserializer_ctx->constant_order_mapping.emplace(static_cast<std::uint8_t>(numeral_right->value), result);
			}
		}
}