#pragma once

#define alu_hilo(M)				\
	M->reg[hi] = Z & UMASK(32, 64);		\
	M->reg[lo] = Z & LMASK(32);

#define alu_binary(M, S, A, B, O)			\
	S##32 X = A ? (S##32)M->reg[A] : 0;		\
	S##32 Y = B ? (S##32)M->reg[B] : 0;		\
	S##64 Z;					\
        Z = X O Y;					

#define alu_regimm(M, S, A, O)				\
	S##32 X = A ? (S##32)M->reg[A] : 0;		\
	S##16 Y = i->imm;				\
	S##64 Z;					\
        Z = X O Y;

#define alu_special2(M, S, A, B, O1, O2)		\
	S##32 X = A ? (S##32)M->reg[A] : 0;		\
	S##32 Y = B ? (S##32)M->reg[B] : 0;		\
	S##64 Z = M->reg[hi] | M->reg[lo];		\
        Z O2 X O1 Y;					

#define alu_special2_hilo(M, S, A, B, O1, O2)		\
	{						\
		alu_special2(M, S, A, B, O1, O2);	\
		alu_hilo(M);				\
	}						\

#define alu_regimm_trap(M, S, A, E, O)		\
	{					\
		alu_regimm(M, S, A, O);		\
		if (Z) {			\
			exception(M, E);	\
			return EXCEPTION;	\
		}				\
	}

#define alu_regimm_reg(M, S, A, B, O)		\
	{					\
		alu_regimm(M, S, A, O);		\
		M->reg[B] = Z;			\
	}					

#define alu_binary_hilo(M, S, A, B, O)			\
	{						\
		alu_binary(M, S, A, B, O);		\
		alu_hilo(M);				\
	}

#define alu_binary_reg(M, S, A, B, C, O)		\
	{						\
		alu_binary(M, S, A, B, O);		\
		M->reg[C] = Z;				\
	}

#define alu_binary_trap(M, S, A, B, E, O)		\
	{						\
		alu_binary(M, S, A, B, O)		\
		if (Z) {				\
			exception(M, E);		\
			return EXCEPTION;		\
		}					\
	}						

#define alu_binary_branch(M, S, A, B, O)				\
	{								\
		alu_binary(M, S, A, B, O);				\
		int R = cpu_step(M, 1);					\
		if (R) {						\
			return R;					\
		}							\
		if (Z) {						\
			i16 I = i->imm;					\
			M->reg[pc] += I << 2;				\
		}							\
	}

#define alu_binary_branch_link(M, S, A, B, O)	\
	if (A == 31) { return EINVINST; }	\
	M->reg[31] = M->reg[pc] + 4;		\
	alu_binary_branch(M, S, A, B, O);

#define alu_unary(M, S, A, O)			\
	S##32 X = A ? (S##32)M->reg[A] : 0;	\
	S##64 Z;				\
	Z = O(X);				

#define alu_unary_reg(M, S, A, O)		\
	{					\
		alu_unary(M, S, A, O);		\
		M->reg[A] = Z;			\
	}

#define exception(M, E) M->cp0.cause_reg.exc = E;

#define mem_load(M, S, A, B, C)						\
	{								\
		S##B X = 0;						\
		S##32 Z;						\
		if (A % 2) {						\
			exception(M, address_load_exc);			\
		}							\
		int ret = vmemcpy(M->mem, A, &X, B/8, TO_HOST);		\
		if (ret) { return ret; }				\
		Z = X;							\
		M->reg[C] = Z;						\
	}

#define mem_store(M, S, A, B, C)					\
	{								\
		S##B X = M->reg[C];					\
		S##32 Z;						\
		if (A % 2) {						\
			exception(M, address_load_exc);			\
		}							\
		Z = X;							\
		int ret = vmemcpy(M->mem, A, &Z, B/8, TO_VM);		\
		if (ret) { return ret; }				\
	}
