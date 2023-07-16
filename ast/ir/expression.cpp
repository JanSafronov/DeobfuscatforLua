#include "expression.hpp"
#include "abstract_visitor_pattern.hpp"

#include <sstream>
#include <algorithm>
#include <iterator>

// we dont have to deal with multirets here, hence why we won't use std::transform.
namespace deobf::ast::ir::expression {
	const std::unordered_map<binary_expression::operation_t, std::string>
		binary_expression::operation_map {
			// logical
			{ operation_t::and, "and" },
			{ operation_t::or, "or" },

			// basic math
			{ operation_t::add, "+" },
			{ operation_t::sub, "-" },
			{ operation_t::mul, "*" },
			{ operation_t::mod, "%" },
			{ operation_t::div, "/" },
			{ operation_t::pow, "^" },

			// string manip
			{ operation_t::concat, ".." },

			// comparsion
			{ operation_t::le, "<=" },
			{ operation_t::lt, "<" },
			{ operation_t::gt, ">" },
			{ operation_t::ge, ">=" },
			{ operation_t::eq, "==" },
			{ operation_t::neq, "~=" },
	};

	const std::unordered_map<unary_expression::operation_t, std::string>
		unary_expression::operation_map{
			{ operation_t::len, "#" },
			{ operation_t::minus, "-" },
			{ operation_t::not, "not" },
	};

	// basic literal types

	bool numeral_literal::equals(const node* other_node) const {
		return false;
	}

	void numeral_literal::accept(abstract_visitor_pattern* visitor) {
		visitor->accept(this);
	}

	bool boolean_literal::equals(const node* other_node) const {
		return false;
	}

	void boolean_literal::accept(abstract_visitor_pattern* visitor) {
		visitor->accept(this);
	}

	bool string_literal::equals(const node* other_node) const {
		return false;
	}

	void string_literal::accept(abstract_visitor_pattern* visitor) {
		visitor->accept(this);
	}

	void nil_literal::accept(abstract_visitor_pattern* visitor) {
		visitor->accept(this);
	}

	void literal_base::accept(abstract_visitor_pattern* visitor) { // removing would boost speed in optimizations?
		visitor->accept(this);
	}

	void array_index_literal::accept(abstract_visitor_pattern* visitor) { // removing would boost speed in optimizations?
		visitor->accept(this);
	}
	// binary expression

	std::string binary_expression::to_string() const {
		std::ostringstream stream;

		stream << "( ";
		
		if (const auto result = operation_map.find(operation); result != operation_map.cend())
			stream << left->to_string() << " " << result->second << " " << right->to_string();
		
		stream << " )";

		return stream.str();
	}

	std::vector<std::shared_ptr<node>> binary_expression::get_children() const {
		return { left, right };
	}

	void binary_expression::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) { // does it accept the expression?, if so tell expressions to accept the visitor
			left->accept(visitor);
			right->accept(visitor);
		}
	}
	// unary expression

	std::string unary_expression::to_string() const {
		if (const auto result = operation_map.find(operation); result != operation_map.cend())
			return result->second + "( " + body->to_string() + " )";

		return "( )";
	}

	bool unary_expression::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> unary_expression::get_children() const {
		return { body };
	}

	void unary_expression::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this))
			body->accept(visitor);
	}

	// table

	std::string table::to_string() const {
		std::ostringstream stream;
		stream << "{ ";

		for (auto& [key, value] : entries) {
			stream << '[' << key->to_string() << "] = " << value->to_string() << ", ";
		}
		stream << "}";

	
		return stream.str();
	}

	bool table::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> table::get_children() const
	{
		auto children_vector = std::vector<std::shared_ptr<node>>{ };

		for (auto& [key, value] : entries) {
			children_vector.push_back(key);
			children_vector.push_back(value);
		}

		return children_vector;
	}
}