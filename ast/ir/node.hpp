#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <functional>

namespace deobf::ast::ir {
	struct abstract_visitor_pattern; // visitor dependency

	// polymorphic clone CRTP
	template <class derived>
	struct cloneable_node : node {
		virtual std::unique_ptr<derived> clone() const {
			return std::make_unique<derived>(static_cast<const derived&>(*this));
		}
	};

	struct node : public std::enable_shared_from_this<node> {
		virtual std::string to_string() const = 0; // just like in Lua.g4 antlr form
		
		[[nodiscard]] virtual bool equals(const node* other_node) const = 0; // useless, deprecated in our IR for now (todo operator==(const node& other_node) instead?)

		virtual std::vector<std::shared_ptr<node>> get_children() const {
			return { };
		};

	};
}