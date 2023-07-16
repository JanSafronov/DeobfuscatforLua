#pragma once
#include <variant>
#include <string>
#include <functional>
#include <type_traits>

#include "vm_arch/instruction.hpp"

// credits to da stack overflow post for the is_contained pattern

template <typename T, typename ...>
struct is_contained : std::false_type { };

template <typename T, typename Head, typename ...Tail>
struct is_contained<T, Head, Tail...> : std::integral_constant<bool, std::is_same<T, Head>::value || is_contained<T, Tail...>::value> { };

namespace deobf::vm_arch {

	struct constant {
		// todo references for DFA? not redudrant atm
		std::vector<std::reference_wrapper<const vm_arch::instruction>> owners;

		enum class constant_type : short {
			nil, // todo do we really have to hold NIL ?

			number,
			string,
			boolean,
		};
	};
}