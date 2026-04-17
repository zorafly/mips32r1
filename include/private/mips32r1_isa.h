INSTRUCTION(add)
{
	/* CERT: Signed Integer Overflow */
	if ((((i32)m->reg[i->rt] > 0) &&
	     ((i32)m->reg[i->rs] > I32_MAX - (i32)m->reg[i->rt])) ||
	    (((i32)m->reg[i->rt] < 0) &&
	     ((i32)m->reg[i->rs] < I32_MIN - (i32)m->reg[i->rt]))) {
		exception(m, intov_exc);
		return EXCEPTION;
	}
	alu_binary_reg(m, i, i->rs, i->rt, i->rd, +);
	return SUCCESS;
}

INSTRUCTION(addi)
{
	alu_regimm_reg(m, i, i->rs, i->rt, +);
	return SUCCESS;
}

INSTRUCTION(addiu)
{
	alu_regimm_reg(m, u, i->rs, i->rt, +);
	return SUCCESS;
}

INSTRUCTION(addu)
{
	alu_binary_reg(m, u, i->rs, i->rt, i->rd, +);
	return SUCCESS;
}

INSTRUCTION(and)
{
	alu_binary_reg(m, u, i->rs, i->rt, i->rd, &);
	return SUCCESS;
}

INSTRUCTION(andi)
{
	alu_regimm_reg(m, i, i->rs, i->rt, &);
	return SUCCESS;
}

INSTRUCTION(beq)
{
	alu_binary_branch(m, u, i->rs, i->rt, ==);
	return SUCCESS;
}

INSTRUCTION(bgezal)
{
	alu_binary_branch_link(m, i, i->rs, 0, >=);
	return SUCCESS;
}


INSTRUCTION(bgez)
{
	alu_binary_branch(m, i, i->rs, 0, >=);
	return SUCCESS;
}

INSTRUCTION(bgtz)
{
	alu_binary_branch(m, u, i->rs, 0, >);
	return SUCCESS;
}

INSTRUCTION(blez)
{
	alu_binary_branch(m, u, i->rs, 0, <=);
	return SUCCESS;
}

INSTRUCTION(bltzal)
{
	alu_binary_branch_link(m, i, i->rs, 0, <);
	return SUCCESS;
}

INSTRUCTION(bltz)
{
	alu_binary_branch(m, i, i->rs, 0, <);
	return SUCCESS;
}

INSTRUCTION(bne)
{
	alu_binary_branch(m, u, i->rs, i->rt, !=);
	return SUCCESS;
}

INSTRUCTION(brkp)
{
	m->cp0.cause_reg.exc = brkp_exc;
	return EXCEPTION;
}

INSTRUCTION(cache)
{
	exception(m, unusable_exc);
	return EXCEPTION;
}

INSTRUCTION(clo)
{
	unsigned x;
	unsigned c = 0;
	if (!m->reg[i->rs]) {
		m->reg[i->rd] = 32;
		return SUCCESS;
	}
	for (x = 0; x < 32; x++) {
		if (!(m->reg[i->rs] & (1 << x))) {
			break;
		}
		c++;
	}
	m->reg[i->rd] = c;
	return SUCCESS;
}

INSTRUCTION(clz)
{
	unsigned x;
	unsigned c = 0;
	if (!m->reg[i->rs]) {
		m->reg[i->rd] = 32;
		return SUCCESS;
	}
	for (x = 0; x < 32; x++) {
		if (m->reg[i->rs] & (1 << x)) {
			break;
		}
		c++;
	}
	m->reg[i->rd] = c;
	return SUCCESS;
}

INSTRUCTION(div)
{
	if ((!m->reg[i->rt]) ||
	    (((i32)m->reg[i->rs] == I32_MIN) && ((i32)m->reg[i->rt] == -1))) {
		    exception(m, intov_exc);
		    return EXCEPTION;
	    }
	alu_binary_reg(m, i, i->rs, i->rt, lo, /);
	alu_binary_reg(m, i, i->rs, i->rt, hi, %); 
	return SUCCESS;
}

INSTRUCTION(divu)
{
	alu_binary_reg(m, u, i->rs, i->rt, lo, /);
	alu_binary_reg(m, u, i->rs, i->rt, hi, %); 
	return SUCCESS;
}

INSTRUCTION(eret)
{
	m->mode = user_mode;
	m->reg[pc] = m->cp0.epc_reg;
	return SUCCESS;
}

INSTRUCTION(jal)
{
	u32 link = m->reg[pc] + 4;
	u32 addr = (m->reg[pc] & 0xf0000000) | (i->address << 2);
	int ret = cpu_step(m, 1);
	if (ret) {
		return ret;
	}
	m->reg[31] = link;
	m->reg[pc] = addr;
	return SUCCESS;
}

INSTRUCTION(jalr)
{
	int ret;
	u32 x;
	/* Validate the instruction */
	if (i->rs == i->rd) {
		return EINVARG;
	}
	/* Branch instructions not allowed in delay slots */
	if (delay) {
		return EINVINST;
	}
	/* Calculate the return address */
	x = m->reg[pc] + 8;
	/* Validate the address */
	if (x % 4) {
		return EINVADDR;
	}
	/* Execute the branch delay slot */
	ret = cpu_step(m, 1);
	if (ret < 0) {
		return ret;
	}
	if (ret) {
		return cpu_handle_exception(m, delay);
	}
	/* Store the return address */
	m->reg[i->rd] = x;
	/* Perform the jump */
	m->reg[pc] = i->address;
	return SUCCESS;
}

INSTRUCTION(j)
{
	u32 addr = (m->reg[pc] & 0xf0000000) | (i->address << 2);
	int ret = cpu_step(m, 1);
	if (ret) {
		return ret;
	}
	m->reg[pc] = addr;
	return SUCCESS;
}

INSTRUCTION(jr)
{
	u32 x = m->reg[i->rs];
	if (x % 4) {
		return EINVADDR;
	}
	if (delay) {
		return EINVINST;
	}
	int ret = cpu_step(m, 1);
	if (ret) {
		return ret;
	}
	m->reg[pc] = x;
	return SUCCESS;
}

INSTRUCTION(lb)
{
	mem_load(m, i, m->reg[i->base] + i->offset, 8, i->rt); 
	return SUCCESS;
}

INSTRUCTION(lbu)
{
	mem_load(m, u, m->reg[i->base] + i->offset, 8, i->rt); 
	return SUCCESS;
}

INSTRUCTION(lh)
{
	mem_load(m, i, m->reg[i->base] + i->offset, 16, i->rt); 
	return SUCCESS;
}

INSTRUCTION(lhu)
{
	mem_load(m, u, m->reg[i->base] + i->offset, 16, i->rt); 
	return SUCCESS;
}

INSTRUCTION(lui)
{
	u32 imm = i->imm;
	m->reg[i->rt] = imm << 16;
	return SUCCESS;
}

INSTRUCTION(lw)
{
	return vmemcpy(m->mem, m->reg[i->base] + (i16)i->offset, &m->reg[i->rt],
		       4, TO_HOST);
}

INSTRUCTION(lwl)
{
	int ret;
	u32 x;
	u32 y = m->reg[i->rt];
	ret = vmemcpy(m->mem, m->reg[i->base] + (i16)i->offset, &x, 4, TO_HOST);
	if (ret) {
		return ret;
	}
	m->reg[i->rt] = (x & 0xff00) | (y & 0xff);
	return SUCCESS;
}

INSTRUCTION(lwr)
{
	int ret;
	u32 x = m->reg[i->rt];
	u32 y;
	ret = vmemcpy(m->mem, m->reg[i->base] + (i16)i->offset, &y, 4, TO_HOST);
	if (ret) {
		return ret;
	}
	m->reg[i->rt] = (x & 0xff00) | (y & 0xff);
	return SUCCESS;
}

INSTRUCTION(madd)
{
	alu_special2_hilo(m, i, i->rs, i->rt, *, +=);
	return SUCCESS;
}

INSTRUCTION(maddu)
{
	alu_special2_hilo(m, u, i->rs, i->rt, *, +=);
	return SUCCESS;
}

INSTRUCTION(mfc0)
{
	u32* reg;
	int ret = cpu_get_cp0reg(m, &reg, i->rd, i->sel);
	if (ret || !reg) {
		return SUCCESS;
	}
	memcpy(&m->reg[i->rt], reg, 4);
	return SUCCESS;
}

INSTRUCTION(mfhi)
{
	m->reg[i->rd] = m->reg[hi];
	return SUCCESS;
}

INSTRUCTION(mflo)
{
	m->reg[i->rd] = m->reg[lo];
	return SUCCESS;
}

INSTRUCTION(movn)
{
	if (m->reg[i->rt]) {
		m->reg[i->rd] = m->reg[i->rs];
	}
	return SUCCESS;
}

INSTRUCTION(movz)
{
	if (!m->reg[i->rt]) {
		m->reg[i->rd] = m->reg[i->rs];
	}
	return SUCCESS;
}

INSTRUCTION(msub)
{
	alu_special2_hilo(m, i, i->rs, i->rt, *, -=);
	return SUCCESS;
}

INSTRUCTION(msubu)
{
	alu_special2_hilo(m, u, i->rs, i->rt, *, -=);
	return SUCCESS;
}

INSTRUCTION(mtc0)
{
	u32* reg;
	int ret = cpu_get_cp0reg(m, &reg, i->rd, i->sel);
	if (ret || !reg) {
		return SUCCESS;
	}
	if (!i->rt) {
		*reg = 0;
	}
	else {
		memcpy(reg, &m->reg[i->rt], 4);
	}
	return SUCCESS;
}

INSTRUCTION(mthi)
{
	m->reg[hi] = m->reg[i->rs];
	return SUCCESS;
}

INSTRUCTION(mtlo)
{
	m->reg[lo] = m->reg[i->rs];
	return SUCCESS;
}

INSTRUCTION(mul)
{	
	alu_binary_reg(m, i, i->rs, i->rt, i->rd, *);
	return SUCCESS;
}

INSTRUCTION(mult)
{
	alu_binary_hilo(m, i, i->rs, i->rt, *);
	return SUCCESS;
}

INSTRUCTION(multu)
{
	alu_binary_hilo(m, u, i->rs, i->rt, *);
	return SUCCESS;
}

INSTRUCTION(nor)
{
	alu_binary_reg(m, u, i->rs, i->rt, i->rd, |);
	alu_unary_reg(m, u, i->rd, -);
	return SUCCESS;
}

INSTRUCTION(or)
{
	alu_binary_reg(m, u, i->rs, i->rt, i->rd, |);
	return SUCCESS;
}

INSTRUCTION(ori)
{
	alu_regimm_reg(m, i, i->rs, i->rt, |);
	return SUCCESS;
}

INSTRUCTION(sb)
{
	mem_store(m, i, m->reg[i->base] + i->offset, 8, i->rt); 
	return SUCCESS;
}

INSTRUCTION(sh)
{
	mem_store(m, i, m->reg[i->base] + i->offset, 16, i->rt); 
	return SUCCESS;
}

INSTRUCTION(sll)
{
	m->reg[i->rd] = m->reg[i->rt] << i->sa;
	return SUCCESS;
}

INSTRUCTION(sllv)
{
	m->reg[i->rd] = m->reg[i->rt] << (m->reg[i->rs] & 31);
	return SUCCESS;
}

INSTRUCTION(slt)
{
	alu_binary_reg(m, i, i->rs, i->rt, i->rd, <);
	return SUCCESS;
}

INSTRUCTION(slti)
{
	alu_regimm_reg(m, i, i->rs, i->rt, <);
	return SUCCESS;
}

INSTRUCTION(sltiu)
{
	alu_regimm_reg(m, u, i->rs, i->rt, <);
	return SUCCESS;
}

INSTRUCTION(sltu)
{
	alu_binary_reg(m, u, i->rs, i->rt, i->rd, <);
	return SUCCESS;
}

INSTRUCTION(sra)
{
	u32 x = m->reg[i->rt];
	u32 s = -(x >> 31);
	m->reg[i->rd] = (s ^ x) >> i->sa ^ s;
	return SUCCESS;
}

INSTRUCTION(srav)
{
	u32 x = m->reg[i->rt];
	u32 s = -(x >> 31);
	m->reg[i->rd] = (s ^ x) >> (m->reg[i->rs] & 31) ^ s;
	return SUCCESS;
}

INSTRUCTION(srl)
{
	m->reg[i->rd] = m->reg[i->rt] >> i->sa;
	return SUCCESS;
}

INSTRUCTION(sub)
{
	/* CERT: Signed Integer Overflow */
	if ((((i32)m->reg[i->rt] > 0) &&
	     ((i32)m->reg[i->rs] < I32_MIN + (i32)m->reg[i->rt])) ||
	    (((i32)m->reg[i->rt] < 0) &&
	     ((i32)m->reg[i->rs] > I32_MAX + (i32)m->reg[i->rt]))) {
		exception(m, intov_exc);
		return EXCEPTION;
	}
	alu_binary_reg(m, i, i->rs, i->rt, i->rd, -);
	return SUCCESS;
}

INSTRUCTION(subu)
{
	alu_binary_reg(m, u, i->rs, i->rt, i->rd, -);
	return SUCCESS;
}

INSTRUCTION(sw)
{
	return vmemcpy(m->mem, m->reg[i->base] + (i16)i->offset, &m->reg[i->rt],
		       4, TO_VM);
}

INSTRUCTION(swl)
{
	return vmemcpy(m->mem, m->reg[i->base] + (i16)i->offset, &m->reg[i->rt],
		       2, TO_VM);
}

INSTRUCTION(swr)
{
	return vmemcpy(m->mem, m->reg[i->base] + (i16)i->offset + 2,
		       &m->reg[i->rt], 2, TO_VM);
}

INSTRUCTION(sync)
{
	return SUCCESS;
}

INSTRUCTION(syscall)
{
	m->cp0.cause_reg.exc = syscall_exc;
	return EXCEPTION;
}

INSTRUCTION(teq)
{
	alu_binary_trap(m, i, i->rs, i->rt, trap_exc, ==);
	return SUCCESS;
}

INSTRUCTION(teqi)
{
	alu_regimm_trap(m, i, i->rs, trap_exc, ==);
	return SUCCESS;
}

INSTRUCTION(tge)
{
	alu_binary_trap(m, i, i->rs, i->rt, trap_exc, >=);
	return SUCCESS;
}

INSTRUCTION(tgei)
{
	alu_regimm_trap(m, i, i->rs, trap_exc, >=);
	return SUCCESS;
}

INSTRUCTION(tgeiu)
{
	alu_regimm_trap(m, u, i->rs, trap_exc, >=);
	return SUCCESS;
}

INSTRUCTION(tgeu)
{
	alu_binary_trap(m, u, i->rs, i->rt, trap_exc, >=);
	return SUCCESS;
}

INSTRUCTION(tlt)
{
	alu_binary_trap(m, i, i->rs, i->rt, trap_exc, <);
	return SUCCESS;
}

INSTRUCTION(tlti)
{
	alu_regimm_trap(m, i, i->rs, trap_exc, <);
	return SUCCESS;
}

INSTRUCTION(tltiu)
{
	alu_regimm_trap(m, u, i->rs, trap_exc, <);
	return SUCCESS;
}

INSTRUCTION(tltu)
{
	alu_binary_trap(m, u, i->rs, i->rt, trap_exc, <);
	return SUCCESS;
}

INSTRUCTION(tne)
{
	alu_binary_trap(m, i, i->rs, i->rt, trap_exc, !=);
	return SUCCESS;
}

INSTRUCTION(tnei)
{
	alu_regimm_trap(m, i, i->rs, trap_exc, !=);
	return SUCCESS;
}

INSTRUCTION(wait)
{
	return SUCCESS;
}

INSTRUCTION(xor)
{
	alu_binary_reg(m, u, i->rs, i->rt, i->rd, ^);
	return SUCCESS;
}

INSTRUCTION(xori)
{
	alu_regimm_reg(m, i, i->rs, i->rt, ^);
	return SUCCESS;
}
