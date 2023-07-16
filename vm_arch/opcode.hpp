#pragma once

#include <cstdint>
#include <unordered_map>
#include <string_view>

/**
	~ IRONBREW DEOBFUSCATOR VM ARCHITECTURE ~
**/

// operand field size
#define SIZE_C          9
#define SIZE_B          9
#define SIZE_Bx         (SIZE_C + SIZE_B)
#define SIZE_A          8

#define SIZE_OP         6

// operand field positions
#define POS_OP          0
#define POS_A           (POS_OP + SIZE_OP)
#define POS_C           (POS_A + SIZE_A)
#define POS_B           (POS_C + SIZE_C)
#define POS_Bx          POS_C

#define MAXARG_A        ((1<<SIZE_A)-1)
#define MAXARG_B        ((1<<SIZE_B)-1)
#define MAXARG_C        ((1<<SIZE_C)-1)

/* creates a mask with `n' 1 bits at position `p' */
#define MASK1(n,p)      ((~((~(unsigned long)0)<<n))<<p)

#define cast(t, exp)    (static_cast<t>(exp))

#define GETARG_A(i)     (cast(int, ((i)>>POS_A) & MASK1(SIZE_A,0)))

#define GETARG_B(i)     (cast(int, ((i)>>POS_B) & MASK1(SIZE_B,0)))

#define GETARG_C(i)     (cast(int, ((i)>>POS_C) & MASK1(SIZE_C,0)))

#define GETARG_Bx(i)    (cast(int, ((i)>>POS_Bx) & MASK1(SIZE_Bx,0)))

#define MAXARG_sBx 131071

#define GETARG_sBx(i)   (GETARG_Bx(i)-MAXARG_sBx)

#define ENCODE_OPERAND(operand, size, pos) (operand & ((1 << size) - 1)) << pos

namespace deobf::vm_arch {
	enum class opcode : std::uint8_t { // part of the ironbrew ISA, vanilla : https://www.lua.org/source/5.1/lopcodes.h.html
		op_invalid,

		op_move,
		op_loadk,

		op_loadbool,
		op_loadbool1, // enable on branching loadbool (C != 0 -> PC++)

		op_loadnil,
		op_getupval,
		op_getglobal,

		op_gettable,
		op_gettable1, // gettable kst (R[A] := STK[B][RK(C)]) ( ENABLE IF ISK(C) )

		op_setglobal,
		op_setupval,

		// pseudo-instructions

		op_settable,
		op_settable1, // enable if isK(B)
		op_settable2, // enable if isK(C)
		op_settable3, // enable if isK(BC)

		op_newtable,

		op_self,
		op_self1, // enable if isK(C)

		// 3 cases because different generation for basic artimethic instructions (RK(B), RK(C) and stack)  

		op_add,
		op_add1, // enable if isK(B)
		op_add2, // enable if isK(C)
		op_add3, // enable if isK(BC)

		op_sub,
		op_sub1, // enable if isK(B)
		op_sub2, // enable if isK(C)
		op_sub3, // enable if isK(BC)

		op_mul,
		op_mul1, // enable if isK(B)
		op_mul2, // enable if isK(C)
		op_mul3, // enable if isK(BC)

		op_div,
		op_div1, // enable if isK(B)
		op_div2, // enable if isK(C)
		op_div3, // enable if isK(BC)

		op_mod,
		op_mod1, // enable if isK(B)
		op_mod2, // enable if isK(C)
		op_mod3, // enable if isK(BC)

		op_pow,
		op_pow1, // enable if isK(B)
		op_pow2, // enable if isK(C)
		op_pow3, // enable if isK(BC)

		op_unm,
		op_not,
		op_len,
		op_concat,
		op_jmp,

		// eq/lt/le (generated in control structures, nonlogical unlike test/set)
		// have their operand A as RK(B) (because if boolean A is false then we use eq and A=1 has a different opcode)


		op_eq,
		op_eq1, // enable if isK(B)
		op_eq2, // enable if isK(C)
		op_eq3, // enable if isK(BC)

		op_lt,
		op_lt1, // enable if isK(B)
		op_lt2, // enable if isK(C)
		op_lt3, // enable if isK(BC)

		op_le,
		op_le1, // enable if isK(B)
		op_le2, // enable if isK(C)
		op_le3, // enable if isK(BC)
		
		op_test,

		op_testset,

		op_test1, // inverse test (C == 1)
		op_testset1, // inverse testset (C == 1)

		// lt (generated when A==1)
		op_ge,
		op_ge1, // enable if isK(B)
		op_ge2, // enable if isK(C)
		op_ge3, // enable if isK(BC)

		// le (generated when A==1)
		op_gt,
		op_gt1, // enable if isK(B) 
		op_gt2, // enable if isK(C)
		op_gt3, // enable if isK(BC)

		// eq (generated when A==1)
		op_ne,
		op_ne1, // enable if isK(B) 
		op_ne2, // enable if isK(C)
		op_ne3, // enable if isK(BC)

		// god damn it
		op_call, // b > 2, c > 2
		op_call1, // b == 2, c > 2 
		op_call2, // b == 0, c > 2
		op_call3, // b == 1, c > 2
		op_call4, // b > 2, c == 0
		op_call5, // b == 2, c == 0
		op_call6, // b > 2, c == 1
		op_call7, // b == 2, c == 1
		op_call8, // b == 0, c == 0
		op_call9, // b == 0, c == 1
		op_call10, // b == 1, c == 0
		op_call11, // b == 1, c == 1
		op_call12, // b > 2, c == 2
		op_call13, // b == 2, c == 2
		op_call14, // b == 0, c == 2
		op_call15, // b == 1, c == 2

		op_tailcall,
		op_tailcall1, // b == 0 (passed L->top amount of params to call stack)
		op_tailcall2, // b == 1 (0 parameters)

		op_return,
		op_return1, // b == 2, one stack return
		op_return2,  // b == 3, 2 stack returns
		op_return3, // b == 0, obviously L->top returns (could be checked if is preceded by call or vararg because top is set in both)
		op_return4, // b == 1, 0 returns.

		op_forloop,
		op_forprep,
		op_tforloop,

		op_setlist,
		op_setlist1, // B == 0 (insert STK[A][i] := STK[1 <= A+i <= top], aka UNK values.)
		op_setlist2, // C == 0, branching

		op_close,

		op_closure,
		op_closure1, // proto has no upvalues (no MOVEs followed by)

		op_vararg,
		op_vararg1, // enabled if B == 0, L->top amount of varargs are pushed to stack

		// IRONBREW SIMD INSTRUCTION SET
		op_newstack,
		op_settop,

		// IRONBREW DEOBFUSCATOR CUSTOM INSTRUCTION SET
		op_decryptedkstflag,
	};
}