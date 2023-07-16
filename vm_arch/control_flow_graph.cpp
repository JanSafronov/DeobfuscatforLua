#include "control_flow_graph.hpp"

#include <stdexcept>
#include <iostream>

namespace deobf::vm_arch::control_flow_graph {


	std::shared_ptr<basic_block> generate_basic_blocks(block_reference_t& block_references, std::size_t init_label, std::vector<std::unique_ptr<vm_arch::instruction>>& instructions, std::unordered_set<vm_arch::instruction*>& labels) {
		if (const auto result = block_references.find(init_label); result != block_references.cend()) {
			return result->second;
		}
		
		auto entry_block = std::make_shared<basic_block>();

		block_references.emplace(init_label, entry_block);

		for (; init_label < instructions.size(); ++init_label) {
			const auto& current_instruction = instructions.at(init_label);

			current_instruction.get()->print();

			if (current_instruction->op == vm_arch::opcode::op_return
				|| current_instruction->op == vm_arch::opcode::op_return1
				|| current_instruction->op == vm_arch::opcode::op_return2
				|| current_instruction->op == vm_arch::opcode::op_return3
				|| current_instruction->op == vm_arch::opcode::op_return4) { // || current_instruction->op == vm_arch::opcode::op_settop)
				std::cout << "retblock.\n";
				entry_block->instructions.emplace_back(*current_instruction);

				entry_block->is_terminator = true; // flip terminator flag (current terminator instructions : return)
				if (init_label + 1 < instructions.size()) {
					entry_block->next_block = generate_basic_blocks(block_references, init_label + 1, instructions, labels);

					entry_block->block_sucessors.push_back(entry_block->next_block);
					entry_block->next_block->block_predecessors.push_back(entry_block);
				}
				return entry_block;
			}
			else if (current_instruction->is_unconditional_jump()) {
				entry_block->instructions.emplace_back(*current_instruction);

				const auto target_pc = current_instruction->get_target_pc(init_label + 1);

				if (target_pc < 0 || target_pc > instructions.size()) {
					throw std::runtime_error("un/conditional jump target pc is out of bounds");
				}

				const auto target_instruction = instructions.at(target_pc).get();
				if (labels.count(target_instruction)) {
					entry_block->target_block = generate_basic_blocks(block_references, target_pc, instructions, labels);

					entry_block->block_sucessors.push_back(entry_block->target_block);
					entry_block->target_block->block_predecessors.push_back(entry_block);
					
				}

				if (init_label + 1 < instructions.size()) {
					entry_block->next_block = generate_basic_blocks(block_references, init_label + 1, instructions, labels);

					entry_block->block_sucessors.push_back(entry_block->next_block);
					entry_block->next_block->block_predecessors.push_back(entry_block);
				}

				// next basic block starts after current leader

				return entry_block;
			}
			else if (labels.count(current_instruction.get())) {
				if (init_label + 1 < instructions.size() && !entry_block->instructions.empty()) {
					entry_block->instructions.emplace_back(*current_instruction);
					entry_block->next_block = generate_basic_blocks(block_references, init_label + 1, instructions, labels);

					entry_block->block_sucessors.push_back(entry_block->next_block);
					entry_block->next_block->block_predecessors.push_back(entry_block);

					// todo remove?
					return entry_block;
				}
			}
			entry_block->instructions.emplace_back(*current_instruction);
		}

		return entry_block;
	}
}