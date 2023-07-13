#include "mark_deserializer.hpp"
#include "marker_decorator_interface.hpp"

namespace deobf::ironbrew_devirtualizer::devirtualizer_ast_markers {
	using namespace ast;

	struct deserializer_marker_visitor final : marker_decorator_interface {
		using marker_decorator_interface::marker_decorator_interface;

		bool accept(ir::statement::variable_assign* statement) override {
			if (statement->variables.size() == 1 && statement->expressions.size() == 1) {
				if (auto get_call = statement->expressions.at(0)->as<ir::expression::function_call>()) {
					const auto& call_arguments = get_call->arguments.at(0)->body;
					if (call_arguments.size() == 3) {
						std::cout << get_call->name.value()->to_string() << std::endl;
						if (get_call->name.has_value(); get_call->name.value()->to_string() == "string.sub") {
							std::cout << call_arguments.at(0)->to_string() << std::endl;
							if (const auto symbol = current_block->find_symbol(call_arguments.at(0)->to_string())) {
								if (auto call_result = symbol->symbol_value->as<ir::expression::function_call>()) { // bytecode compression on
									const auto vm_string = call_result->arguments.at(0)->body.at(0);

									std::cout << "vm_string:" << vm_string->to_string() << std::endl;

									current_block->insert_symbol<true>("vm_string", vm_string);
								}
								else if (auto string_result = symbol->symbol_value->as<ir::expression::string_literal>()) { // bytecode compression off (edge case)
									std::cout << "vm_string:" << string_result->value << std::endl;

									current_block->insert_symbol<true>("vm_string", string_result);
								}
								return false;
							}
						}
					}
				}
				else if (statement->variables.at(0)->to_string() == "ByteString") { // kinda lazy ik
					current_block->insert_symbol<true>("vm_string", statement->expressions.at(0));
				}
			}
			return true;
		}
	};
}