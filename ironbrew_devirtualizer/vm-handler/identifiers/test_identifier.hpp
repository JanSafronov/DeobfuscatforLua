#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct test_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() == 1) {
				if (auto test_statement = path->find_first_of<ir::statement::if_statement>()) {
				}
			}

			return __super::handle(path);
		}
	};
}