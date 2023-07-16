#pragma once
#include <cstdint>
#include "opcode.hpp"
#include <iostream>
#include <unordered_set>

namespace deobf::vm_arch {

	struct vanilla_instruction final {

		// wanted to use this before but it will get messy to handle on transpilation
		// https://i.imgur.com/rOC1qSo.png
/*#pragma pack(push, 1)
		union {
			struct {
				vm_arch::vanilla_opcode opcode;
				
				struct {
					std::uint8_t a;
					union {
						struct {
							std::uint8_t b;
							std::uint8_t c;
						};
						std::int16_t sbx;
						std::uint16_t bx;
					};
				};
			};
			std::uint32_t value; // is big endian
		};
#pragma pack(pop)*/


		virtual ~vanilla_instruction() = default;
	};
}