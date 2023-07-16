#include "vanilla_lifter.hpp"

namespace deobf::ironbrew_devirtualizer::vanilla_lifter::proto_translator {
	std::unique_ptr<vm_arch::vanilla_proto> translate_chunk(vm_arch::proto* chunk, bool translate_protos) {
		auto new_proto = std::make_unique<vm_arch::vanilla_proto>();

		// todo renumber chunks? (linedefined, lastlinedfined)

		new_proto->is_vararg = chunk->is_vararg;
		new_proto->num_upvalues = chunk->nups;
		new_proto->num_parameters = chunk->num_params;
		new_proto->max_stack_size = chunk->max_stack_size; // todo resolve here on p->code or dynamic analysis?

		// translate embeded protos
		if (translate_protos)
			for (auto& proto : chunk->protos)
				new_proto->protos.emplace_back(translate_chunk(proto.get()));

		return std::move(new_proto);
	}
}