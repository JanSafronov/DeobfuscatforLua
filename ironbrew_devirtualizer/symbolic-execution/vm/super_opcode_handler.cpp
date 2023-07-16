// old code and might have memory leaks
// new uses a SMT solver to generate and simplify handler expressions, aswell as caching handler scopes.

#include "./ast/ir/node.hpp"
#include "opcode_handler_computer.hpp"
#include "ironbrew_devirtualizer/devirtualizer_ast_markers/marker_decorator_interface.hpp"

#include <algorithm>
#include <numeric>
#include <stack>

namespace deobf::ironbrew_devirtualizer::symbolic_execution::vm {
	using namespace ast;

	// optimization passes for instructions aswell before forwarding to handler (removes dead code, unused registers)
	struct instruction_propagator final : devirtualizer_ast_markers::marker_decorator_interface {
		using marker_decorator_interface::marker_decorator_interface;

		bool accept(ir::statement::local_declaration* statement) override {
			auto body_position = std::size_t{ };
			for (auto& body_statement : statement->body) {
				if (const auto result = instruction_indice_mapping.find(body_statement->to_string()); result != instruction_indice_mapping.cend()) {
					const auto variable_name = statement->names.at(body_position)->to_string();
					current_block->find_symbol(variable_name)->resolve_identifier = result->second;
					local_dfs_stack.emplace(statement->shared_from_this());
				}

				++body_position;
			}

			return true;
		}

		bool accept(ir::statement::block* statement) override {
			// rebase block, etc for symbol table puropses.
			if (statement->parent == rebase_group.first && current_block != nullptr) {
				statement->parent = rebase_group.second;
			}

			return __super::accept(statement);
		}

		bool accept(ir::expression::variable* expression) override {
			if (const auto result = instruction_indice_mapping.find(expression->to_string()); result != instruction_indice_mapping.cend()) {
				// transform node to be in its single form
				expression->suffixes.clear();
				expression->name->as<ir::expression::string_literal>()->value = result->second;
			}

			return __super::accept(expression);
		}

		std::stack<std::weak_ptr<ir::node>> local_dfs_stack;
		std::pair<ir::statement::block*, ir::statement::block*> rebase_group;

	private:
		static inline std::unordered_map<std::string_view, std::string_view> instruction_indice_mapping {
			{ "current_instruction[1]", "instruction_opcode_virtual" },
			{ "current_instruction[2]", "instruction_opcode_a" },
			{ "current_instruction[3]", "instruction_opcode_b" },
			{ "current_instruction[4]", "instruction_opcode_c" },
			//{ "current_instruction[5]", "instruction_opcode_unk" },
		};
	};

		flush_body(); // final body

		//body->parent = old_parent;
	}
}
