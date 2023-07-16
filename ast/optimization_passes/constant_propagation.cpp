#include "constant_propagation.hpp"
#include "ast/ir/abstract_visitor_pattern.hpp"
#include "ast/ir/node.hpp"

#include <type_traits>
#include <thread>
#include <mutex>


namespace deobf::ast::optimization_passes {
	struct constant_propagation_visitor final : ir::abstract_visitor_pattern {

		std::optional<std::shared_ptr<ir::expression::expression>> optimize(ir::expression::variable* expression) { // constant propagation
			if (current_visitor_block == nullptr)
				return std::nullopt;

			if (const auto symbol = current_visitor_block->find_symbol(expression->to_string())) {
				if (symbol->is_known_symbol()) {
					return symbol->symbol_value->as<ir::expression_t>();
				}
				else if (symbol->symbol_value->to_string() == "( unpack or table.unpack )") { // some addition ig
					return std::make_shared<ir::expression::string_literal>("unpack");
				}
			}

			return std::nullopt;
		}

		bool accept(ir::expression::function_call* expression) override {
			if (expression->name.has_value()) {
				auto& name_value = expression->name.value();
				if (auto variable_node = dynamic_cast<ir::expression::variable*>(name_value.get())) {
					if (auto optimized_node = optimize(variable_node); optimized_node.has_value()) {
						expression->name = std::move(optimized_node.value());
					}
				}
			}

			return true;
		}

	private:
		const ir::statement::block* current_visitor_block = nullptr;
		//std::once_flag has_visited_return;
	};

	void constant_propagation::optimize() {
		constant_propagation_visitor base;
		root->accept(&base);

	}
}