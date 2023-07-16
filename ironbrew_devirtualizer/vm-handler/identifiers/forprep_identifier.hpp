#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct forprep_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			auto if_stat = path->find_first_of<ir::statement::if_statement>();
			if (if_stat.has_value()) {
			}
			
			return __super::handle(path);
		}
	};
}