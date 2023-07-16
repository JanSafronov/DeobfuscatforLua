#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct tailcall_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			auto do_statement = path->find_first_of<ir::statement::do_block>();
			if (do_statement.has_value()) {
				auto& body_wrapper = do_statement->get();
				std::cout << "dostat\n";
				if (body_wrapper.body->body.size() == 0) {
					auto call_statement = body_wrapper.find_first_of<ir::expression::function_call>();
					if (call_statement.has_value()) {
						auto& call_wrapper = call_statement->get();						
					}
				}
			}

			return __super::handle(path);
		}
	};
}