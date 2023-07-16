#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct return_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() == 1) {
			}

			return __super::handle(path);
		}
	};
}