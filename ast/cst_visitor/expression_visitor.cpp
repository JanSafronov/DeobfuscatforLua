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
}