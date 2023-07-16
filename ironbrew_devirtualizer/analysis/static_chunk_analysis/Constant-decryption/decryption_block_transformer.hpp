#pragma once
#include "vm_arch/control_flow_graph.hpp"
#include "vm_arch/basic_block.hpp"
#include "vm_arch/proto.hpp"
#include "../static_chunk_analysis.hpp"
#include <queue>

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis {
	using namespace vm_arch;

	namespace constant_decryption {
		class decryption_block_transformer final {
			friend struct constant_decryption_recongizer;

			const std::string decrypted_constant; // decrypted string
			const std::int32_t parameter_reference;
		public:

			bool optimize(); // performs dead code elimination and linking begin to end
			
			explicit decryption_block_transformer(const std::string& decrypted_constant, const std::int32_t parameter_reference, static_chunk_analysis& analyzer, std::weak_ptr<basic_block> begin, std::weak_ptr<basic_block> end) :
				decrypted_constant(decrypted_constant),
				parameter_reference(parameter_reference),
				analyzer(analyzer),
				begin(begin),
				end(end)
			{ };
		};
	}
}