#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct self_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			
			auto result = path->get_children_type<ir::statement::variable_assign>();

			std::cout << "sz:" << result.size() << std::endl;

			handle_proc:
			return __super::handle(path);
		}
	};
}