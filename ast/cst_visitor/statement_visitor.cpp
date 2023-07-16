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

	// main
	
	antlrcpp::Any cst_visitor::visitFuncStat(LuaParser::FuncStatContext* ctx) {
		auto function_node = std::move(visitFuncbody(ctx->funcbody()).as<std::shared_ptr<ir::expression::function>>());
		
		function_node->function_name = ctx->funcname()->getText();

		// we dont need to handle global functions in symbol table

		//current_parse_block->insert_symbol(function_node->function_name.value(), function_node);
		
		function_node->type = expression::function::function_type::global_t;

		return std::static_pointer_cast<ir::statement::statement>(function_node);
	}

	antlrcpp::Any cst_visitor::visitLocalFuncStat(LuaParser::LocalFuncStatContext* ctx) {
		std::string function_name = "f" + std::to_string(++function_counter);

		auto symbol = current_parse_block->insert_symbol(ctx->NAME()->getText());
		symbol.first->second->resolve_identifier = function_name;

		auto function_node = visitFuncbody(ctx->funcbody()).as<std::shared_ptr<ir::expression::function>>();

		function_node->function_name = function_name;

		symbol.first->second->symbol_value = function_node;

		current_parse_block->insert_symbol<false>(function_name, function_node);

		function_node->type = expression::function::function_type::local_t;

		return std::static_pointer_cast<ir::statement::statement>(function_node);
	}


	antlrcpp::Any cst_visitor::visitRetstat(LuaParser::RetstatContext* ctx) {

		auto return_parameters = expression::expression_list_t{ };

		if (auto expression_list = ctx->explist())
			return_parameters = visitExplist(ctx->explist()).as<ir::expression::expression_list_t>();

		auto return_statement = std::make_shared<ir::statement::return_statement>(std::move(return_parameters));

		return return_statement;
	}

	antlrcpp::Any cst_visitor::visitRepeatStat(LuaParser::RepeatStatContext* ctx) {

		auto repeat_body = visitBlock(ctx->block()).as<std::shared_ptr<ir::statement::block>>();
		auto repeat_condition = visitExp(ctx->exp()).as<std::shared_ptr<ir::expression::expression>>();

		auto repeat_statement = std::make_shared<ir::statement::repeat>(std::move(repeat_body), std::move(repeat_condition));

		return std::static_pointer_cast<ir::statement::statement>(repeat_statement);
	}

}