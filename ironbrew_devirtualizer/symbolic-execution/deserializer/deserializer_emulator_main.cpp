#include "deserializer_emulator_main.hpp"
#include "vm_arch/proto.hpp"
#include "deserializer_context.hpp"

namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
	std::unique_ptr<vm_arch::proto> deserializer_emulator_main::deserialize() {
        auto proto = std::make_unique<vm_arch::proto>();

        for (const auto current_order : deserializer_ctx->chunk_order) {
            switch (current_order) {
                case deserializer_enums::process_order::constants: {
                    const auto kst_max_size = deserializer_helper_object->get_bits<std::int32_t>();
                    for (auto i = 0l; i < kst_max_size; ++i) {
                        const auto randomized_order_type = deserializer_helper_object->get_bits<std::int8_t>();
                        if (const auto& kst_type = deserializer_ctx->constant_order_mapping.find(randomized_order_type); kst_type != deserializer_ctx->constant_order_mapping.cend()) {
                            switch (kst_type->second) {
                                case deserializer_enums::constant_type::string: {
                                    auto x = deserializer_helper_object->get_string();
                                    std::cout << "result:" << x << std::endl;
                                    proto->constants.push_back(std::make_unique<vm_arch::constant>(x));
                                    break;
                                }
                                case deserializer_enums::constant_type::number: {
                                    proto->constants.push_back(std::make_unique<vm_arch::constant>(deserializer_helper_object->get_float()));
                                    break;
                                }
                                case deserializer_enums::constant_type::boolean: {
                                    proto->constants.push_back(std::make_unique<vm_arch::constant>(static_cast<bool>(deserializer_helper_object->get_bits<std::int8_t>())));
                                    break;
                                }
                                default:
                                    throw std::runtime_error("unknown constant type? [deserializer]");
                            }
                        }
                        else {
                            proto->constants.push_back(std::make_unique<vm_arch::constant>(std::nullptr_t{ }));
                        }
                    }

                    break;
                }
            }
        }

        return std::move(proto);
	}
}
