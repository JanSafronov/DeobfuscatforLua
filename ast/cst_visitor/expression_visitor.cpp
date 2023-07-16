#include "visitor_include.hpp"

#include <algorithm>

namespace deobf::ast {
	using namespace ir;

	antlrcpp::Any cst_visitor::visitTableconstructor(LuaParser::TableconstructorContext* ctx) {
		std::vector<std::pair<std::shared_ptr<expression_t>, std::shared_ptr<expression_t>>> entries;
		auto array_part = static_cast<std::size_t>(0u); // MAX_INT is max for array size in lua

		if (auto field_list = ctx->fieldlist()) {
			for (auto field : field_list->field()) {
				// (field->LSB() exp field->RSB()) OR (ast_string EQL exp) OR exp
				if (field->LSB() != nullptr) {
					auto key = visitExp(field->exp().front()).as<std::shared_ptr<expression_t>>();
					auto value = visitExp(field->exp().back()).as<std::shared_ptr<expression_t>>();

					entries.emplace_back(std::move(key), std::move(value)); // try_emplace for collisions
				}
				else if (auto field_name = field->NAME()) {
					auto key = std::make_shared<expression::string_literal>(field_name->getText());
					auto value = visitExp(field->exp().back()).as<std::shared_ptr<expression_t>>();

					entries.emplace_back(std::move(key), std::move(value));
				}
				else {
					auto array_key = std::make_shared<expression::array_index_literal>(++array_part);
					auto array_value = visitExp(field->exp().back()).as<std::shared_ptr<expression_t>>();

					entries.emplace_back(std::move(array_key), std::move(array_value));
				}
			}
		}

		auto table_node = std::make_shared<expression::table>(std::move(entries), array_part);

		return std::static_pointer_cast<expression_t>(table_node);
	}

	antlrcpp::Any cst_visitor::visitExplist(LuaParser::ExplistContext* ctx) {
		expression::expression_list_t expression_list;

		for (auto expression : ctx->exp()) {
			auto expression_node = visitExp(expression).as<std::shared_ptr<expression_t>>();
			expression_list.push_back(std::move(expression_node));
		}
		
		return expression_list;
	}

	antlrcpp::Any cst_visitor::visitArgs(LuaParser::ArgsContext* ctx) {
		// LP explist? RP | tableconstructor | string
		
		/* if (ctx->LP()) */

		auto arguments = expression::expression_list_t{ };

		if (auto expression_list = ctx->explist()) {
			arguments = visitExplist(ctx->explist()).as<expression::expression_list_t>();
		}
		else if (auto string_argument = ctx->string()) {
			auto string_literal = std::make_shared<expression::string_literal>(string_argument->getText());
			arguments.push_back(std::move(string_literal));
		}
		else if (auto table_constructor = ctx->tableconstructor()) {
			auto table_argument = visitTableconstructor(table_constructor).as<std::shared_ptr<expression_t>>();
			arguments.push_back(std::move(table_argument));
		}

		return arguments;
	}

	antlrcpp::Any cst_visitor::visitNameAndArgs(LuaParser::NameAndArgsContext* ctx) {
		std::string name;
		if (auto raw_name = ctx->NAME())
			name = raw_name->getText();

		auto arguments = visitArgs(ctx->args()).as<expression::expression_list_t>(); //std::variant<expression::expression_list_t, std::shared_ptr<expression_t>>

		auto name_and_args_node = std::make_shared<expression::name_and_args>(name, std::move(arguments));

		return name_and_args_node;
	}
	
	antlrcpp::Any cst_visitor::visitString(LuaParser::StringContext* ctx) {
		const auto string_value = ctx->getText();
		
		std::string init_value; 
		switch (string_value.front()) {
			case '[': {
				const auto start_delimeter = string_value.find('[', 1) + 1;
				const auto end_delimeter = string_value.find_last_of(']', string_value.size() - 2);
				init_value = string_value.substr(start_delimeter, end_delimeter - start_delimeter);
				break;
			}
			case '\'': // unused fallthough fornow
				init_value = string_value.substr(1, string_value.size() - 2);
				break;
			case '"':
				init_value = string_value.substr(1, string_value.size() - 2);
				break;
			default:
				throw std::logic_error("string_literal error");
		}

		auto string_literal = std::make_shared<expression::string_literal>(init_value);

		return std::static_pointer_cast<expression_t>(string_literal);
	}

	antlrcpp::Any cst_visitor::visitFunctioncall(LuaParser::FunctioncallContext* ctx) {
		expression::name_and_args_t arguments;
		for (const auto name : ctx->nameAndArgs()) {
			auto result = visitNameAndArgs(name).as<std::shared_ptr<expression::name_and_args>>();
			arguments.push_back(std::move(result));
		}

		std::shared_ptr<expression_t> expression_name = nullptr;

		auto var_or_exp = ctx->varOrExp();
		if (auto prefix_expression = var_or_exp->exp()) {
			expression_name = visitExp(prefix_expression).as<std::shared_ptr<expression_t>>();
		}
		else if (auto prefix_variable = var_or_exp->var()) {
			auto name = visitVar(prefix_variable).as<std::shared_ptr<expression_t>>();
			expression_name = std::move(name);
		}

		auto call_expression = std::make_shared<expression::function_call>(std::move(expression_name), std::move(arguments));

		return std::static_pointer_cast<statement::statement>(call_expression);
	}

	antlrcpp::Any cst_visitor::visitVarDecl(LuaParser::VarDeclContext* ctx) {
		auto variables = visitVarlist(ctx->varlist()).as<ir::expression::variable_list_t>();

		ir::expression::expression_list_t expressions;
		if (auto expression_list = ctx->explist())
			expressions = visitExplist(expression_list).as<ir::expression::expression_list_t>();

		auto variable_node = std::make_shared<ir::statement::variable_assign>(std::move(variables), std::move(expressions)); // statement

		return std::static_pointer_cast<statement::statement>(variable_node);
	}
}