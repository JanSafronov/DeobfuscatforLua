// visitor pattern dependancy ( this is used to convert the CST into our AST. )
#pragma once

#include <LuaBaseVisitor.h>
#include "ast/ir/statement.hpp"
#include "ast/ir/expression.hpp"

// semantic analysis (AST renaming done for functions, function parameters, variables) is always impelemented via visitor.

// todo fix precedence for function calls? (i.e. the following expression a()() might result in unexpected behaviour), ANTLR grammar issue?

// TODO RENAME FORLOOP VARIABLES

// todo fix multi_statements on visitor, unordered somewhat?

namespace deobf::ast {
	struct cst_visitor final : public LuaBaseVisitor {
	private:
		static std::size_t function_counter, parameter_counter, variable_counter; // used for semantic analysis
		ir::statement::block* current_parse_block = nullptr;

		auto enter_scope(LuaParser::BlockContext* block) {
			const auto temp_old_block = current_parse_block;
			auto generated_block = std::make_shared<ir::statement::block>(current_parse_block); // initialize parent, etc.
			current_parse_block = generated_block.get();

			return generated_block;
		};

		void exit_scope(LuaParser::BlockContext* block) {
			ir::statement::managed_statement_list statements_body;
			for (const auto statement : block->stat()) {
				auto statement_node = visitStat(statement).as<std::shared_ptr<ir::statement::statement>>();
				statements_body.push_back(std::move(statement_node));
			}

			current_parse_block->body = std::move(statements_body);

			if (auto return_statement = block->retstat()) {
				auto return_node = visitRetstat(return_statement).as<std::shared_ptr<ir::statement::return_statement>>();
				current_parse_block->ret = std::move(return_node);
			}

			current_parse_block = current_parse_block->parent;
		}
	};
}