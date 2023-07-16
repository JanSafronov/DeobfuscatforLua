#include "statement.hpp"
#include <sstream>
#include <algorithm>
#include <iterator>
#include "expression.hpp"

#include "abstract_visitor_pattern.hpp"

#include <iostream>

namespace deobf::ast::ir::statement {

	// return statement

	std::string return_statement::to_string() const {
		std::ostringstream stream;

		stream << "return ";
		
		if (!body.empty()) {
			if (body.size() > 1)
				std::transform(body.cbegin(), body.cend() - 1, std::ostream_iterator<std::string>(stream, ","), [&stream](const auto& value) {
					return value->to_string();
				});

			stream << body.back()->to_string();
		}

		return stream.str();
	}

	bool return_statement::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> return_statement::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>{ };

		for (auto& name_and_arg : body)
			children_vector.emplace_back(name_and_arg);

		return children_vector;
	}

	void return_statement::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this))
			for (auto& statement : body)
				statement->accept(visitor);
	}
	// block

	bool block::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> block::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>{ }; // (body.begin(), body.end());
		
		for (auto& statement : body)
			children_vector.push_back(statement);

		if (ret.has_value())
			children_vector.push_back(ret.value());

		return children_vector;
	}
	
	void block::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			for (auto& stat : body) {
				stat->accept(visitor);
			}

			if (ret.has_value()) {
				ret.value()->accept(visitor);
			}
		}
	}
	// do block

	bool do_block::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> do_block::get_children() const {
		return { body };
	}
}