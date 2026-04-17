#pragma once

/* Return mask for `_up' amount of upper bits from a `_bits' wide value */
#define UMASK(_up, _bits) (((1UL << _up) - 1) << (_bits - _up))
/* Return mask for `_low' amount of lower bits from any width value */
#define LMASK(_low) ((1UL << _low) - 1)

/* Virtual memory parameters [see vram_new in mem.h] */
#define mips32r1_width 32
#define mips32r1_pagelen 4096
#define mips32r1_npages 1048576
#define mips32r1_nbuckets 4096

/* Exception base addresses */
#define mips32r1_reset_base 0xbfc00000
/* #define mips32r1_ejtag1_base 0xbfc00480
   #define mips32r1_ejtag2_base 0xff200200
   #define mips32r1_cache_error_base 0xa0000000 */
#define mips32r1_other_base 0x80000000

/* Exception vector offsets */
/* #define mips32r1_tlb_refill_off 0
   #define mips32r1_cache_error_off 0x100*/
#define mips32r1_interrupt_off 0x200
#define mips32r1_reset_off 0

/* Exception vectors */
#define mips32r1_reset_vector 0xbfc00000
/* #define mips32r1_ejtag1_vector 0xbfc00480
   #define mips32r1_ejtag2_vector 0xbfc00200 */
#define mips32r1_interrupt1_vector 0x80000200
#define mips32r1_interrupt2_vector 0xbfc00380

enum mips32r1_mode {
	debug_mode = 0,
	kernel_mode = 1,
	user_mode = 2
};

enum mips32r1_exceptions {
	interrupt_exc     = 0,
	tlb_mod_exc       = 1,
	tlb_load_exc      = 2,
	tlb_store_exc     = 3,
	address_load_exc  = 4,
	address_store_exc = 5,
	bus_fetch_exc     = 6,
	bus_data_exc      = 7,
	syscall_exc       = 8,
	brkp_exc          = 9,
	reserved_exc      = 10,
	unusable_exc      = 11,
	intov_exc         = 12,
	trap_exc          = 13,
	msafp_exc         = 14,
	fp_exc            = 15,
	/* 16-17 implementation defined */
	c2_exc            = 18,
	tlbri_exc         = 19,
	tlbxi_exc         = 20,
	msadis_exc        = 21,
	mdmx_exc          = 22,
	watch_exc         = 23,
	mcheck_exc        = 24,
	thread_exc        = 25, /* MT module */
	dspdis_exc        = 26, /* DSP module */
	ge_exc            = 27,
	/* 28-29 reserved */
	cache_exc         = 28,
	/* 31 reserved */
};


/* Indices of special registers in the register array */
enum mips32r1_special_regs {
	pc = 32,
	lo = 33,
	hi = 34
};

/* To encode: u16 = regnum | ((selnum + 1) << 8) */
enum mips32r1_cp0_regs {
	badvaddr = 264,
	count = 265,
	/* 1801, 2057 reserved for implementation-dependent usage */
	compare = 3082,
	status = 268,
	cause = 269,
	epc = 270,
	prid = 271,
	config0 = 272,
	config1 = 528,
	errorepc = 286	
};

struct mips32r1_cp0 {
	union {
		u32 regs[10];
		struct {
			u32 badvaddr_reg;
			u32 count_reg;
			u32 compare_reg;
			union {
				u32 status_reg_raw;
				struct {
					bits ie : 1;
					bits exl : 1;
					bits erl : 1;
					union {
						struct {
							bits r0 : 1;
							bits um : 1;
						};
						bits ksu : 2;
					};
					bits res0 : 3;
					bits im0 : 8;
					bits im1 : 9;
					union {
						struct {
							bits im2 : 1;
							bits im3 : 1;
							bits im4 : 1;
							bits im5 : 1;
							bits im6 : 1;
							bits im7 : 1;
						};
						bits ipl : 6;
					};
					bits impl : 2;
					bits ase : 1;
					bits nmi : 1;
					bits sr : 1;
					bits ts : 1;
					bits bev : 1;
					bits res1 : 1;
					bits mx : 1;
					bits re : 1;
					bits fr : 1;
					bits rp : 1;
					bits cu0 : 1;
					bits cu1 : 1;
					bits cu2 : 1;
					bits cu3 : 1;
				} status_reg;
			};
			union {
				u32 cause_reg_raw;
				struct {
					bits res2 : 1;
					bits exc : 4;
					bits res3 : 2;
					bits ip0 : 1;
					bits ip1 : 1;
					union {
						struct {
							bits ip2 : 1;
							bits ip3 : 1;
							bits ip4 : 1;
							bits ip5 : 1;
							bits ip6 : 1;
							bits ip7 : 1;
						};
						bits ripl : 6;
					};
					bits ase1 : 2;
					bits res4 : 3;
					bits fdci : 1;
					bits wp : 1;
					bits iv : 1;
					bits ase2 : 2;
					bits pci : 1;
					bits dc : 1;
					bits ce : 2;
					bits ti : 1;
					bits bd : 1;
				} cause_reg;
			};
			u32 epc_reg;
			union {
				u32 prid_reg_raw;
				struct {
					u8 rev;
					u8 id;
					u8 cid;
					u8 copt;
				} prid_reg;
			};
			union {
				u32 config0_reg_raw;
				struct {
					bits k0 : 2;
					bits vi : 1;
					bits res5 : 3;
					bits mt : 3;
					bits ar : 3;
					bits at : 2;
					bits be : 1;
					bits impl : 9;
					bits ku : 3;
					bits k23 : 3;
					bits m : 1;
				} config0_reg;
			};
			union {
				u32 config1_reg_raw;
				struct {
					bits fp : 1;
					bits ep : 1;
					bits ca : 1;
					bits wr : 1;
					bits pc : 1;
					bits md : 1;
					bits c2 : 1;
					bits da : 3;
					bits dl : 3;
					bits ds : 3;
					bits ia : 3;
					bits il : 3;
					bits is : 3;
					bits mmusz : 6;
					bits m : 1;
				} config1_reg;
			};
			u32 errorepc_reg;
		};
	};
};

struct mips32r1_machine {
	u32 reg[35];
	union {
		float freg[2];
		double dreg;
		u32 raw[2];
	} fpr[16];
	struct mips32r1_cp0 cp0;
	struct vram* mem;
	u8 mode;
	int debug;
	u64 step;
	u64 limit;
};

/* Generic, decoded instruction */
struct mips32r1_generic {
	u8 sel;
	u8 funct;
	u8 sa;
	u8 rd;
	u8 rt;
	u8 rs;
	u16 imm;			
	u32 address;
	u16 offset;
	u8 base;
	u8 op;
	u8 hint;
	u8 co;
};

/* Packed, encoded instruction */
union mips32r1_instruction {
	struct {
		union {
			struct {
				bits sel     : 3;
				bits pad0    : 23;
			}; /* cop0 */
			struct {
				bits funct   : 6;
				bits sa      : 5;
				bits rd      : 5;
				bits rt      : 5;
				bits rs      : 5;
			}; /* type R */
			struct {
				bits imm     : 16;
				bits pad1    : 10;
			}; /* type I */
			struct {
				bits address : 26;
			}; /* type J */
			struct {
				bits offset  : 16;
				bits pad2    : 5;
				bits base    : 5;
			}; /* load/store */
		};
		bits op : 6;
	}; 
	u32 raw;
};

int mips32r1_dump(struct mips32r1_machine* m, char** out);
int mips32r1_dump_file(struct mips32r1_machine* m, FILE* out);
int mips32r1_new(struct mips32r1_machine** out);
void mips32r1_free(struct mips32r1_machine* in);
int mips32r1_load(struct mips32r1_machine* out, u32 off, u32 start, void* in,
		 size_t inlen);
int mips32r1_run(struct mips32r1_machine* m, u64 limit, int debug);
const char* mips32r1_strerror(int ret);

