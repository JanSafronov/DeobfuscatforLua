#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct setlist_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (auto for_step = path->find_first_of<ir::statement::for_step>(); for_step.has_value()) {
				auto function_call = for_step.value().get().body->find_first_of<ir::expression::function_call>();
				if (function_call.has_value()) {
					if (auto function_name = function_call->get().name; function_name.has_value()) {
					}
				}
			}

			return __super::handle(path);
		}
	};
}