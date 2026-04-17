#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "error.h"
#include "mem.h"
#include "mips32r1.h"
#include "mips32r1_encoding.h"
#include "cpu.h"
#include "instruction_headers.h"
#include "mips32r1_isa.h"
#include "instruction_table.h"
#include "instruction_names.h"
#include "debug.h"

const char* debug_special =
	"instruction: SPECIAL: %s \n"
	"\tfunct: %u || sa: %u || rd: %u || rt: %u || rs: %u || op: %u";

const char* debug_loadstore =
	"instruction: LOAD/STORE: %s\n"
	"\toffset: %x || rt: %u || base: %u || op: %u";

const char* debug_loadimm =
	"instruction: LOAD IMMEDIATE: %s\n"
	"\timm: %x || rt: %u || op: %u";

const char* debug_immop =
	"instruction: IMMOP: %s\n"
	"\timm: %x || rt %u || rs: %u || op: %u";

const char* debug_branch =
	"instruction: BRANCH: %s\n"
	"\toffset: %x || rs: %u || op: %u";

const char* debug_jump =
	"instruction: JUMP: %s\n"
	"\taddress: %x || op: %u";

const char* debug_jalr =
	"instruction: JALR\n"
	"\thint: %u || rd: %u || rs: %u";

const char* debug_jr =
	"instruction: JR\n"
	"\thint: %u || rs: %u";

static int valid_ranges(u64 a, unsigned n, ...)
{
	unsigned i;
	va_list ap;
	u64 l;
	u64 h;
	int ret = 0;
	va_start(ap, n);
	for (i = 0; i < n; i++) {
		l = va_arg(ap, u64);
		h = va_arg(ap, u64);
		if (a >= l && a <= h) {
			ret++;
			break;
		}
	}
	va_end(ap);
	return ret;
}

static int cpu_decode_special(executor* out, struct mips32r1_generic* g, u32 in)
{
	u32 t = in;
	g->funct = t & LMASK(6);

	if (valid_ranges(g->funct, 8,
			 5, 5,
			 14, 14,
			 20, 23,
			 28, 31,
			 40, 41,
			 44, 47,
			 53, 53,
			 55, 63)) {
		return ENYI;
	}
	t >>= 6;	
	g->sa = t & LMASK(5);
	t >>= 5;
	g->rd = t & LMASK(5);
	t >>= 5;
	g->rt = t & LMASK(5);
	t >>= 5;
	g->rs = t & LMASK(5);

	log_debug(debug_special, special_names_table[g->funct],
		  g->funct, g->sa, g->rd, g->rt, g->rs, g->op);
	
	*out = special_table[g->funct];
	return SUCCESS;
}

static int cpu_decode_regimm(executor* out, struct mips32r1_generic* g, u32 in)
{
	u32 t = in;
	g->offset = t & LMASK(16);
	t >>= 16;
	g->rt = t & LMASK(5);
	t >>= 5;
	g->rs = t & LMASK(5);
	
	if (valid_ranges(g->rt, 4,
			 4, 7,
			 13, 13,
			 15, 15,
			 20, 31)) {
		return ENYI;
	}
	*out = regimm_table[g->rt];
	return SUCCESS;
}

static int cpu_decode_special2(executor* out, struct mips32r1_generic* g,
			       u32 in)
{
	u32 t = in;
	g->funct = t & LMASK(6);
	t >>= 16;
	g->rt = t & LMASK(5);
	t >>= 5;
	g->rs = t & LMASK(5);
	
	if (valid_ranges(g->funct, 4,
			 3, 3,
			 6, 31,
			 34, 62,
			 63, 63)) {
		return ENYI;
	}
	*out = special2_table[g->funct];
	return SUCCESS;
}

static int cpu_decode_cop0(executor* out, struct mips32r1_generic* g, u32 in)
{

	u32 t = in;

	if (t & (1 << 25)) {
		g->funct = t & LMASK(6);
		if (valid_ranges(g->funct, 3,
				 0, 23,
				 25, 31,
				 33, 63)) {
			return ENYI;
		}
		*out = cop0_co_table[g->funct];
		return SUCCESS;
	}

	g->sel = t & LMASK(3);
	t >>= 11;
	g->rd = t & LMASK(5);
	t >>= 5;
	g->rt = t & LMASK(5);
	t >>= 5;
	g->rs = t & LMASK(5);
	
	if (valid_ranges(g->rs, 2,
			 1, 3,
			 5, 15)) {
		return ENYI;
	}
	*out = cop0_table[g->rs];
	return SUCCESS;
}

static int cpu_decode_loadstore(executor* out, struct mips32r1_generic* g,
				u32 in)
{
	u32 t = in;
	g->offset = t & LMASK(16);
	t >>= 16;
	g->rt = t & LMASK(5);
	t >>= 5;
	g->base = t & LMASK(5);

	log_debug(debug_loadstore, opcode_names_table[g->op], g->offset,
		  g->rt, g->base, g->op);
	
	*out = opcode_table[g->op];
	return SUCCESS;
}

static int cpu_decode_loadimm(executor* out, struct mips32r1_generic* g,
			      u32 in)
{
	u32 t = in;
	g->imm = t & LMASK(16);
	t >>= 16;
	g->rt = t & LMASK(5);

	log_debug(debug_loadimm, opcode_names_table[g->op], g->imm, g->rt,
		  g->op);
	
	*out = opcode_table[g->op];
	return SUCCESS;
}

static int cpu_decode_immop(executor* out, struct mips32r1_generic* g,
			    u32 in)
{
	u32 t = in;
	g->imm = t & LMASK(16);
	t >>= 16;
	g->rt = t & LMASK(5);
	t >>= 5;
	g->rs = t & LMASK(5);

	log_debug(debug_immop, opcode_names_table[g->op], g->imm, g->rt,
		  g->rs, g->op);
	
	*out = opcode_table[g->op];
	return SUCCESS;
}

static int cpu_decode_branch(executor* out, struct mips32r1_generic* g,
			     u32 in)
{
	u32 t = in;
	g->offset = t & LMASK(16);
	t >>= 21;
	g->rs = t & LMASK(5);

	log_debug(debug_branch, opcode_names_table[g->op], g->offset, g->rs,
		  g->op);
	
	*out = opcode_table[g->op];
	return SUCCESS;
}

static int cpu_decode_jump(executor* out, struct mips32r1_generic* g,
			   u32 in)
{
	u32 t = in;
	g->address = t & LMASK(26);

	log_debug(debug_jump, opcode_names_table[g->op], g->address, g->op);
	
	*out = opcode_table[g->op];
	return SUCCESS;
}

static int cpu_fetch(struct mips32r1_machine* m, void* out)
{
	int ret;
	if (!m || !out) {
		return ENULLPTR;
	}
	if (m->reg[pc] % 4) {
		return EINVADDR;
	}
	ret = vmemcpy(m->mem, m->reg[pc], out, 4, TO_HOST);
	if (ret) {
		return ret;
	}
	m->reg[pc] += 4;
	return SUCCESS;
}

static int cpu_decode(struct mips32r1_machine* m, executor* out,
		      struct mips32r1_generic* g, u32 in)
{
	if (!out) {
		return ENULLPTR;
	}
	g->op = in >> 26;
	if (valid_ranges(g->op, 10,
			 17, 19,
			 24, 27,
			 29, 31,
			 39, 39,
			 44, 45,
			 50, 50,
			 52, 52,
			 54, 55,
			 58, 60,
			 62, 63)) {
		return EINVINST;
	}
	switch(g->op) {
	case special:
		return cpu_decode_special(out, g, in);
	case regimm:
		return cpu_decode_regimm(out, g, in);
	case special2:
		return cpu_decode_special2(out, g, in);
	case cop0:
		return cpu_decode_cop0(out, g, in);
	case lb:
	case lh:
	case lwl:
	case lw:
	case lbu:
	case lhu:
	case lwr:
	case sb:
	case sh:
	case swl:
	case sw:
	case swr:
		return cpu_decode_loadstore(out, g, in);
	case lui:
		return cpu_decode_loadimm(out, g, in);
	case addi:
	case addiu:
	case slti:
	case sltiu:
	case andi:
	case ori:
	case xori:
		return cpu_decode_immop(out, g, in);
	case beq:
	case bne:
	case blez:
	case bgtz:
	case beql:
	case bnel:
	case blezl:
	case bgtzl:
		return cpu_decode_branch(out, g, in);
	case j:
	case jal:
		return cpu_decode_jump(out, g, in);
	}
	log_debug("opcode: %s <%u>", opcode_names_table[g->op], g->op);
	*out = opcode_table[g->op];
	return SUCCESS;
}

int cpu_get_cp0reg(struct mips32r1_machine* m, u32** out, u8 reg, u8 sel)
{
	u16 id;
	if (!out || !m) {
		return ENULLPTR;
	}
	id = reg | ((sel + 1) <<8);
	switch(id) {
	case badvaddr:
		*out = &m->cp0.badvaddr_reg;
		return SUCCESS;
	case count:
		*out = &m->cp0.count_reg;
		return SUCCESS;
	case compare:
		*out = &m->cp0.compare_reg;
		return SUCCESS;
	case status:
		*out = &m->cp0.status_reg_raw;
		return SUCCESS;
	case cause:
		*out = &m->cp0.cause_reg_raw;
		return SUCCESS;
	case epc:
		*out = &m->cp0.epc_reg;
		return SUCCESS;
	case prid:
		*out = &m->cp0.prid_reg_raw;
		return SUCCESS;
	case config0:
		*out = &m->cp0.config0_reg_raw;
		return SUCCESS;
	case config1:
		*out = &m->cp0.config1_reg_raw;
		return SUCCESS;
	case errorepc:
		*out = &m->cp0.errorepc_reg;
		return SUCCESS;
	}
	fprintf(stderr, "id %u reg %u sel %u\n", id, reg, sel);
	return EINVARG;
}

int cpu_handle_exception(struct mips32r1_machine* m, int delay)
{
	/* The CE flag indicates which coprocessor is associated with
	   a Coprocessor Unusable exception 
	   m->cp0.cause_reg.ce;
	   The exc flag contains the actual exception ID
	   m->cp0.cause_reg.exc; */
	u32 off;
	u32 base;
	/* Reduce delay to a boolean (0/1) */
	delay &= 1;
	/* CPU Enter Kernel Mode */
	m->mode = kernel_mode;
	m->cp0.status_reg.exl = 1;
	m->cp0.cause_reg.bd = (delay * m->cp0.status_reg.exl) +
		m->cp0.cause_reg.bd * (~m->cp0.status_reg.exl);	
	/* if (delay) { epc = pc - 4; } else { epc = pc; } */
	m->cp0.epc_reg = ((m->reg[pc] - (4 * delay)) * m->cp0.status_reg.exl) +
		m->cp0.epc_reg * (~m->cp0.status_reg.exl);
	/* Calculate exception handler offset and base */
	off = 384 + (128 * m->cp0.cause_reg.iv);	
	base = 0xbfc00200 + (0x3fc00200 * (~m->cp0.status_reg.bev));
	/* Jump to exception handler */
	m->reg[pc] = base + off;
	return SUCCESS;
}

int cpu_step(struct mips32r1_machine* m, int delay)
{
	executor e;
	u32 raw;
	struct mips32r1_generic g = {0};
	int ret;
	if (!m) {
		return ENULLPTR;
	}
	if (m->step >= m->limit) {
		log_debug("hit cycle limit");
		return QUIT;
	}
	(m->step)++;

	if (m->debug) {
		mips32r1_dump_file(m, stderr);
	}
	
	ret = cpu_fetch(m, &raw);
	if (ret) {
		return ret;
	}

	log_debug("raw instruction: %08x", raw);
	
	if (!raw) {
		log_debug("NOP");
		return SUCCESS;
	}

	if (raw == 0xffffffff) {
		log_debug("halting at empty region");
		return QUIT;
	}
	
	ret = cpu_decode(m, &e, &g, raw);

	if (ret) {
		if (ret < 0) {
			return ret;
		}
		/* Reserved/Unuseable/Not Implemented */
		goto exception;
	}
	if (!e) {
		return ENYI;
	}
	
	ret = e(m, &g, delay);
	if (!ret) {
		return SUCCESS;
	}
	/* Internal error occurred */
	if (ret < 0) {
		return ret;
	}
exception:
	return cpu_handle_exception(m, delay);
}
