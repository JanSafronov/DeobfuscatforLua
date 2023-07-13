#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct div_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() != 1 && path->body.at(0)->is<ir::statement::variable_assign>()) {
				const auto assign_string = path->body.at(0)->to_string();
			}

			return __super::handle(path);
		}
	};
}