#include "auxiliaries.hpp"

namespace deobf::vm_arch::aux {
    static const std::unordered_map<vm_arch::opcode, vm_arch::opcode> kst_optimized_bidict {
        { vm_arch::opcode::op_ne, vm_arch::opcode::op_eq },
        { vm_arch::opcode::op_ne1, vm_arch::opcode::op_eq1 },
        { vm_arch::opcode::op_ne2, vm_arch::opcode::op_eq2 },
        { vm_arch::opcode::op_ne3, vm_arch::opcode::op_eq3 },

        { vm_arch::opcode::op_ge, vm_arch::opcode::op_lt },
        { vm_arch::opcode::op_ge1, vm_arch::opcode::op_lt1 },
        { vm_arch::opcode::op_ge2, vm_arch::opcode::op_lt2 },
        { vm_arch::opcode::op_ge3, vm_arch::opcode::op_lt3 },

        { vm_arch::opcode::op_gt, vm_arch::opcode::op_le },
        { vm_arch::opcode::op_gt1, vm_arch::opcode::op_le1 },
        { vm_arch::opcode::op_gt2, vm_arch::opcode::op_le2 },
        { vm_arch::opcode::op_gt3, vm_arch::opcode::op_le3 },

        { vm_arch::opcode::op_test1, vm_arch::opcode::op_test },
        { vm_arch::opcode::op_testset1, vm_arch::opcode::op_testset },

        // bidrectional mapping
        { vm_arch::opcode::op_lt, vm_arch::opcode::op_ge },
        { vm_arch::opcode::op_lt1, vm_arch::opcode::op_ge1 },
        { vm_arch::opcode::op_lt2, vm_arch::opcode::op_ge2 },
        { vm_arch::opcode::op_lt3, vm_arch::opcode::op_ge3 },

        { vm_arch::opcode::op_le, vm_arch::opcode::op_gt },
        { vm_arch::opcode::op_le1, vm_arch::opcode::op_gt1 },
        { vm_arch::opcode::op_le2, vm_arch::opcode::op_gt2 },
        { vm_arch::opcode::op_le3, vm_arch::opcode::op_gt3 },

        { vm_arch::opcode::op_eq, vm_arch::opcode::op_ne },
        { vm_arch::opcode::op_eq1, vm_arch::opcode::op_ne1 },
        { vm_arch::opcode::op_eq2, vm_arch::opcode::op_ne2 },
        { vm_arch::opcode::op_eq3, vm_arch::opcode::op_ne3 },

        { vm_arch::opcode::op_test, vm_arch::opcode::op_test1 },
        { vm_arch::opcode::op_testset, vm_arch::opcode::op_testset1 },
    };


    static const std::unordered_map<vm_arch::opcode, vm_arch::opcode> normalized_opcode_map {
        { vm_arch::opcode::op_eq, vm_arch::opcode::op_eq },
        { vm_arch::opcode::op_eq1, vm_arch::opcode::op_eq },
        { vm_arch::opcode::op_eq2, vm_arch::opcode::op_eq },
        { vm_arch::opcode::op_eq3, vm_arch::opcode::op_eq },

        { vm_arch::opcode::op_ge, vm_arch::opcode::op_ge },
        { vm_arch::opcode::op_ge1, vm_arch::opcode::op_ge },
        { vm_arch::opcode::op_ge2, vm_arch::opcode::op_ge },
        { vm_arch::opcode::op_ge3, vm_arch::opcode::op_ge },

        { vm_arch::opcode::op_gt1, vm_arch::opcode::op_gt },
        { vm_arch::opcode::op_gt2, vm_arch::opcode::op_gt },
        { vm_arch::opcode::op_gt3, vm_arch::opcode::op_gt },

        { vm_arch::opcode::op_lt, vm_arch::opcode::op_lt },
        { vm_arch::opcode::op_lt1, vm_arch::opcode::op_lt },
        { vm_arch::opcode::op_lt2, vm_arch::opcode::op_lt },
        { vm_arch::opcode::op_lt3, vm_arch::opcode::op_lt },

        { vm_arch::opcode::op_le, vm_arch::opcode::op_le },
        { vm_arch::opcode::op_le1, vm_arch::opcode::op_le },
        { vm_arch::opcode::op_le2, vm_arch::opcode::op_le },
        { vm_arch::opcode::op_le3, vm_arch::opcode::op_le },

        { vm_arch::opcode::op_ne, vm_arch::opcode::op_ne },
        { vm_arch::opcode::op_ne1, vm_arch::opcode::op_ne },
        { vm_arch::opcode::op_ne2, vm_arch::opcode::op_ne },
        { vm_arch::opcode::op_ne3, vm_arch::opcode::op_ne },
    };
}
