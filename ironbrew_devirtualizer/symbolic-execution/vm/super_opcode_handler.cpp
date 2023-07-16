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

	// handles opcodes & superops from body, and forwards them to callback_functor
	void opcode_handler_computer::handle(vm_arch::instruction& instruction, ir::statement::block* body) { // instruction = std::ref(chunk.at(x))
		using name_type = typename ir::expression::name_list_t::value_type;
		using variable_type = typename ir::expression::variable_list_t::value_type;
		
		static instruction_propagator propagator_visitor{ "instruction_opcode_virtual", "instruction_opcode_a", "instruction_opcode_b", "instruction_opcode_c" };


		//const auto old_parent = body->parent;

		auto new_block = std::make_unique<ir::statement::block>(body); // parent is body for symbol search cases
		
		//body->parent = new_block.get();

		propagator_visitor.rebase_group.first = body;

		// IMPORTANT : make sure we have a symbol renamer before using this (important for producing hashes of declarations)
		auto super_op_references = std::unordered_set<std::string>{ }; // stores hashes of declarations inside.

		// memoization being done on visitor

		memoized_virtuals.try_emplace(instruction.virtual_opcode, std::vector<vm_arch::opcode>{ });

		const auto flush_body = [this, &new_block, &body, main_virtual = instruction.virtual_opcode]() {
			const auto result = new_block.get();

			if (!back_track.empty()) {
				auto front_opcode = back_track.front();
				back_track.pop_front();

				// propagate instructions.
				
				propagator_visitor.rebase_group.second = result;
				result->accept(&propagator_visitor);

				
				// handle garbage locals (todo better way?)
				for (; !propagator_visitor.local_dfs_stack.empty(); propagator_visitor.local_dfs_stack.pop()) {
					auto local = propagator_visitor.local_dfs_stack.top();
					if (!local.expired()) {
						auto shared_statement = local.lock().get();

						if (auto iterator_result = std::find_if(result->body.begin(), result->body.end(), [&shared_statement](std::shared_ptr<ir::statement::statement> const& ptr) {
								return shared_statement == ptr.get();
							}); iterator_result != result->body.end()) {
							result->body.erase(iterator_result);
						}
					}
				}

				//body->parent = old_parent;

				const auto opcode_result = std::invoke(callback_functor, front_opcode, result); // forward result to client.
				if (opcode_result == vm_arch::opcode::op_invalid) {
					throw std::runtime_error("invalid opcode?");
				}

				memoized_virtuals.at(main_virtual).push_back(opcode_result);
			}

			new_block.reset(new ir::statement::block); // todo new ir::statement::block(body) instead so we wont have to rebase? maybe reliable?
		};

		flush_body(); // final body

		//body->parent = old_parent;
	}
}
