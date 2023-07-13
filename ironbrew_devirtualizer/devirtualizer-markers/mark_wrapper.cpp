#include "mark_wrapper.hpp"
#include "marker_decorator_interface.hpp"
#include <mutex>

namespace deobf::ironbrew_devirtualizer::devirtualizer_ast_markers {

	// marks constants, instr, protos, parameters, instruction point, vararg size, stack, upvalues, top.

	struct mark_wrapper_visitor final : devirtualizer_ast_markers::marker_decorator_interface {
		using marker_decorator_interface::marker_decorator_interface;
		
		std::once_flag has_visited_for;

		static inline std::unordered_map<std::string_view, std::string_view> result_indice_mapping {
			{ "deserialize_result[1]", "instructions" },
			{ "deserialize_result[2]", "constants" },
			{ "deserialize_result[3]", "protos" }, // will that be different when theres no deserialize_result[4] ?
			{ "deserialize_result[4]", "parameters" },
			{ "1", "instruction_pointer" },
			{ "-( 1 )", "stack_top" },
		};

		bool accept(ir::statement::local_declaration* statement) override {
			if (statement->names.size() == 1 && statement->body.size() == 1) {
				//statement->body.at(0)->accept(this);

				auto variable_name = statement->names.at(0)->to_string();
				auto body_string = statement->body.at(0)->to_string();

				if (const auto symbol = current_block->find_symbol(body_string)) {
					std::cout << symbol->symbol_value->to_string() << std::endl;
					if (const auto result = result_indice_mapping.find(symbol->symbol_value->to_string()); result != result_indice_mapping.cend()) {
						current_block->find_symbol(variable_name)->resolve_identifier = result->second;
						result_indice_mapping.erase(body_string);
					}
					else if (auto binary_expression = statement->body.at(0)->as<ir::expression::binary_expression>()) {
						using operation_t = typename ir::expression::binary_expression::operation_t;

						if (binary_expression->operation == operation_t::add && binary_expression->right->to_string() == "1") {
							if (auto left_branch = binary_expression->left->as<ir::expression::binary_expression>()) {
								if (left_branch->right->to_string() == "protos" || left_branch->right->to_string() == "parameters") {
									current_block->find_symbol(variable_name)->resolve_identifier = "vararg_size";
								}
							}
						}
					}
				}
				// other stuff
				else if (body_string == "pack_return") {
					current_block->find_symbol(variable_name)->resolve_identifier = body_string;
				}
				else if (const auto result = result_indice_mapping.find(body_string); result != result_indice_mapping.cend()) {
					current_block->find_symbol(variable_name)->resolve_identifier = result->second;
					result_indice_mapping.erase(body_string);
				}
			}

			return true;
		}
	};
}