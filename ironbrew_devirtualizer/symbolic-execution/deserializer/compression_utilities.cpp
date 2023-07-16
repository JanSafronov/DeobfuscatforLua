#include "compression_utilities.hpp"

#include <string_view>

namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
    // REWRITEETEITEITIE TODO
    const std::string compression_utilities::decompress_vm_string(std::string_view vm_string) { // shit code dealwit
        std::ostringstream result;
        std::istringstream compression_stream{ vm_string.data() };
        auto conversion_table = std::vector<std::string>{ };

        for (auto i = 0ul; i < 256ul; ++i)
            conversion_table.emplace_back(1, static_cast<char>(i));

        static const auto base36_map = std::string{ "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };

        return result.str();
    }
}
