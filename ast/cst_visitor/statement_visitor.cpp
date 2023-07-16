#include "visitor_include.hpp"

namespace deobf::ast {
	using namespace ir;

	std::size_t cst_visitor::function_counter, cst_visitor::parameter_counter, cst_visitor::variable_counter;

	// wanted to use shared_ptr, since antlrcpp's Any variant type doesn't support copy semantics im going with shared.
	antlrcpp::Any cst_visitor::visitChunk(LuaParser::ChunkContext* ctx) {
		return visitBlock(ctx->block());
	}

	antlrcpp::Any cst_visitor::visitBlock(LuaParser::BlockContext* ctx) {

		// block enter

		const auto temp_old_block = current_parse_block;
		auto generated_block = std::make_shared<ir::statement::block>(current_parse_block); // initialize parent, etc.
		current_parse_block = generated_block.get();

		ir::statement::managed_statement_list statements_body;
		for (const auto statement : ctx->stat()) {
			if (statement->SEMI() != nullptr) // handle temp memory? (todo delete?)
				continue;

			auto statement_node = visitStat(statement).as<std::shared_ptr<ir::statement::statement>>();
			statements_body.push_back(std::move(statement_node));
		}

		generated_block->body = std::move(statements_body);

		if (auto return_statement = ctx->retstat()) {
			auto return_node = visitRetstat(return_statement).as<std::shared_ptr<ir::statement::return_statement>>();
			generated_block->ret = std::move(return_node);
			//return std::make_shared<ir::statement::block>(std::move(statements_body), std::move(return_node));
		}

		// todo : clear our used identifiers ?

		current_parse_block = temp_old_block;

		return std::move(generated_block);
	}
}