#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct close_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (auto for_loop = path->find_first_of<ir::statement::for_step>(); for_loop.has_value()) {
				if (for_loop->get().init->to_string() == "1" && !for_loop->get().step.has_value()) {
				}
			}

			return __super::handle(path);
		}
	};
}