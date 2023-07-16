#include "deserializer_helper.hpp"
#include "deserializer_context.hpp"

namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {

    // todo switch to template overrides? has some bad cons

    const inline bool deserializer_helper::get_bits(std::size_t number, std::size_t i) const {
        const double bit_mask = std::pow(2, i - 1);
        return std::fmodl(number, bit_mask + bit_mask) >= bit_mask;
    }

    const inline std::size_t deserializer_helper::get_bits(std::size_t number, std::size_t i, std::size_t j) const {
        // local l = (number / (2 ^ (i - 1))) % (2 ^ ((j - 1) - (i - 1) + 1))
        const auto lhs = number / std::pow(2, i - 1);
        const auto rhs = std::pow(2, j + 1 - i);

        return std::fmodf(lhs, rhs);
    }
    


    // TODO FIX THIS GETFLOAT
    const double deserializer_helper::get_float() { // update about this: reinterpret_cast'ing into double* and reading wouldn't work even though those are different ieee standards, so we have to rely on the impelemented way
        const auto left = get_bits<std::int32_t>();
        const auto right = get_bits<std::int32_t>();

        bool is_normal_flag = true;
        
        const std::uint64_t mantissa = get_bits(right, 1, 20) * 4294967296 + left;
        auto exponent = get_bits(right, 21, 31);
        const auto sign = std::powl(-1, get_bits(right, 32));

        if (exponent == 0) {
            if (mantissa == 0) {
                return 0;
            }
            else {
                exponent = 1;
                is_normal_flag = false;
            }
        }
        else if (exponent == 2047) {
            if (mantissa == 0) {
                return std::numeric_limits<double>::infinity(); // inf
            }
            else {
                return std::numeric_limits<double>::signaling_NaN(); // return signaling nan (accept exceptions from fpu on ops)
            }
        }

        return std::ldexp(sign, exponent - 1023) * (is_normal_flag + (mantissa / 4.50359963e15));
    }
}