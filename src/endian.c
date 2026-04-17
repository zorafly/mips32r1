#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "types.h"
#include "mips32r1.h"

const union {
	uint32_t i;
	uint8_t c[4];
} e = { 0x01000000 };

int endianness;

/* The following three functions byte-swap types between big and little
   endian formats */
static void order16(void* in)
{
	uint16_t* p = in;
	uint8_t t;
	union {
		uint16_t u16;
		uint8_t u8[2];
	} x;
	x.u16 = *p;
	t = x.u8[0];
	x.u8[0] = x.u8[1];
	x.u8[1] = t;
	memcpy(in, &x, 2);
}

static void order32(void* in)
{
	uint32_t* p = in;
	uint8_t r[4];
	uint8_t a;
	uint8_t b;
	union {
		uint32_t u32;
		uint8_t u8[4];
	} x;
	x.u32 = *p;
	b = 3;
	for (a = 0; a < 4; a++) {
		r[a] = x.u8[b];
		b--;
	}
	memcpy(in, r, 4);
}

static void order64(void* in)
{
	uint64_t* p = in;
	uint8_t r[8];
	uint8_t a;
	uint8_t b;
	union {
		uint64_t u64;
		uint8_t u8[8];
	} x;
	x.u64 = *p;
	b = 7;
	for (a = 0; a < 8; a++) {
		r[a] = x.u8[b];
		b--;
	}
	memcpy(in, r, 8);
}

typedef void (*orderfunc)(void*);
static orderfunc orderfunc_choose(unsigned bits)
{
	switch(bits) {
	case 16: return order16; 
	case 32: return order32; 
	case 64: return order64; 
	default: return NULL;
	}
}

int endian_test()
{
	if (e.c[0]) {
		return 1;
	}
	return 0;
}

void endian_convert(unsigned bits, unsigned nints, ...)
{
	unsigned i;
	va_list ap;
	orderfunc f = orderfunc_choose(bits);
	if (!f) {
		return;
	}
	va_start(ap, nints);
	for (i = 0; i < nints; i++) {
		f(va_arg(ap, void*));
	}
	va_end(ap);
}

