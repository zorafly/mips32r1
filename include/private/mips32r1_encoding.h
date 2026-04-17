#pragma once

/* Encoding of opcode field */
enum mips32r1_enc_opcode {
	special = 0,
	regimm = 1,
	j = 2,
	jal = 3,
	beq = 4,
	bne = 5,
	blez = 6,
	bgtz = 7,
	addi = 8,
	addiu = 9,
	slti = 10,
	sltiu = 11,
	andi = 12,
	ori = 13,
	xori = 14,
	lui = 15,
	cop0 = 16,
	/* cop1 = 17, */
	/* cop2 = 18, */
	/* cop3 = 19, */
	beql = 20,
	bnel = 21,
	blezl = 22,
	bgtzl = 23,
	/* 24-27 */
	special2 = 28,
	/* jalx = 29, */
	/* msa = 30, */
	/* special3 = 31, */
	lb = 32,
	lh = 33,
	lwl = 34,
	lw = 35,
	lbu = 36,
	lhu = 37,
	lwr = 38,
	/* 39 */
	sb = 40,
	sh = 41,
	swl = 42,
	sw = 43,
	/* 44-45 */
	swr = 46,
	cache = 47,
	ll = 48,
	lwc1 = 49,
	/* lwc2 = 50, */
	pref = 51,
	/* 52 */ 
	ldc1 = 53,
	/* ldc2 = 54, */
	/* 55 */
	sc = 56,
	swc1 = 57,
	/* swc2 = 58, */
	/* 59-60 */
	sdc1 = 61,
	/* sdc2 = 62, */
	/* 63 */
};
/* Encoding of funct field for SPECIAL opcode */
enum mips32r1_enc_special_funct {
	sll = 0,
	movci = 1,
	srl = 2,
	sra = 3,
	sllv = 4,
	/* 5 */
	srlv = 6,
	srav = 7,
	jr = 8,
	jalr = 9,
	movz = 10,
	movn = 11,
	syscall = 12,
	brkp = 13,
	/* 14 */
	sync = 15,
	mfhi = 16,
	mthi = 17,
	mflo = 18,
	mtlo = 19,
	/* 20-23 */
	mult = 24,
	multu = 25,
	div = 26,
	divu = 27,
	/* 28-31 */
	add = 32,
	addu = 33,
	sub = 34,
	subu = 35,
	and = 36,
	or = 37,
	xor = 38,
	nor = 39,
	/* 40-41 */
	slt = 42,
	sltu = 43,
	/* 44-47 */
	tge = 48,
	tgeu = 49,
	tlt = 50,
	tltu = 51,
	teq = 52,
	/* 53 */
	tne = 54,
	/* 55-63 */
};
/* Encoding of funct field for SPECIAL2 opcode */
enum mips32r1_enc_special2_funct {
	madd = 0,
	maddu = 1,
	mul = 2,
	/* 3 */
	msub = 4,
	msubu = 5,
	/* 6-31 */
	clz = 32,
	clo = 33,
	/* 34-62 */
	/* sdbbp = 63 */
};
/* Encoding of rt field for REGIMM opcode */
enum mips32r1_enc_regimm_rt {
	bltz = 0,
	bgez = 1,
	bltzl = 2,
	bgezl = 3,
	/* 4-7 */
	tgei = 8,
	tgeiu = 9,
	tlti = 10,
	tltiu = 11,
	teqi = 12,
	/* 13 */
	tnei = 14,
	/* 15 */
	bltzal = 16,
	bgezal = 17,
	bltzall = 18,
	bgezall = 19,
	/* 20-31 */	
};
/* Encoding of rs field for COP0 opcode */
enum mips1_enc_cop0_rs {
	mfc0 = 0,
	/* 1-3 */
	mtc0 = 4,
	/* 5-15 */
};

/* Encoding of function field for COP0 opcode when bit 25 is set */
enum mips1_enc_cop0_funct {
	/* 0 - 23 */
	eret = 24,
	/* 25 - 31 */
	wait = 32,
	/* 33 - 63 */
};


