#pragma once

#include <deque>

#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"
#include "../../../vm_arch/instruction.hpp"

#include <mutex>
#include <functional>


namespace deobf::ironbrew_devirtualizer {
	namespace vm_handler_identifiers {
		struct opcode_identifier_handler;
	}
}

namespace deobf::ironbrew_devirtualizer::symbolic_execution::vm {
	using namespace ast;

	/*
		aztupbrew and few more obfuscators use a loop unrolled, inlined version (divide and conquer) version of binary search in their VM Loop for better performance (though it causes branch mispredictions).
		meaning mapping proccess will be for us O(k * log n) where K is deque size and n is number of branches, this problem is NP hard.
		this one is better than feeding all accepted conditions into a SAT Solver and finding the ONLY `N` that can be evaluted.
		notice that we also can boost the search speed by a little bit via constructing a BST off the visitor pattern.

		algorithm Analysis:

		- Best case complexity : O(k) search time complexity
		- Worst case complexity : O(k * log n) search time complexity
		
		(notice that in lua, the cyclomatic complexity is the same as the time complexity we have)

		- Space complexity is O(1) in both cases.
	*/
}