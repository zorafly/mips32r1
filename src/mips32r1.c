#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "error.h"
#include "mem.h"
#include "mips32r1.h"
#include "cpu.h"

int mips32r1_new(struct mips32r1_machine** out)
{
	int ret;
	struct mips32r1_machine* m;
	if (!out) {
		return ENULLPTR;
	}
	m = malloc(sizeof(struct mips32r1_machine));
	if (!m) {
		return EALLOC;
	}
	memset(m, 0, sizeof(struct mips32r1_machine));
	ret = vram_new(&m->mem, mips32r1_width, mips32r1_pagelen,
		       mips32r1_npages, mips32r1_nbuckets);
	if (ret) {
		free(m);
		return ret;
	}
	fprintf(stderr, "aslen %lu\n", m->mem->aslen);
	*out = m;
	return SUCCESS;
}

void mips32r1_free(struct mips32r1_machine* in)
{
	if (!in) {
		return;
	}
	if (in->mem) {
		vram_free(in->mem);
	}
	free(in);
}

int mips32r1_load(struct mips32r1_machine* out, u32 off, u32 start, void* in,
		 size_t inlen)
{
	if (!out) {
		return ENULLPTR;
	}
	out->reg[pc] = start;
	return vmemcpy(out->mem, off, in, inlen, TO_VM);
}

int mips32r1_dump(struct mips32r1_machine* m, char** out)
{
	int i;
	int ret;
	char s[4096] = {0};
	int p = 0;
	int line = 0;
	if (!m || !out) {
		return ENULLPTR;
	}
	for (i = 0; i < 35; i++) {
		ret = snprintf(&s[p], 4096 - p, "[R%02u :: %08x] ", i,
			       m->reg[i]);
		if (ret < 0) {
			return ret;
		}
		p += ret;

		line++;
		if (line < 4) {
			continue;
		}
		line = 0;
		
		ret = snprintf(&s[p], 4096 - p, "\n");
		if (ret < 0) {
			return ret;
		}
		p += ret;
	}
	ret = snprintf(&s[p], 4096 - p, "\n");
	if (ret < 0) {
		return ret;
	}
	p += ret;
	*out = malloc(4096);
	if (!*out) {
		return EALLOC;
	}
	memset(*out, 0, 4096);
	memcpy(*out, s, p);
	return p;
}

int mips32r1_dump_file(struct mips32r1_machine* m, FILE* out)
{
	char* dump;
	int ret;
	int bytes;
	if (!m || !out) {
		return ENULLPTR;
	}
	ret = mips32r1_dump(m, &dump);
	if (ret < 0) {
		return ret;
	}
	bytes = fwrite(dump, ret, 1, out);
	free(dump);
	if (bytes != ret) {
		return 1;
	}
	return SUCCESS;
}

const char* mips32r1_strerror(int ret)
{
	switch(ret) {
	case ENULLPTR: return "Null pointer dereference";
	case EALLOC: return "Dynamic allocation failure";
	case EBUFOV: return "Buffer overflow";
	case EBADLOC: return "Bad location";
	case EINTOV: return "Integer overflow";
	case EEMPTY: return "Unexpected empty buffer";
	case EFULL: return "Unexpected full buffer";
	case ENYI: return "Not yet implemented";
	case QUIT: return "Quitting";
	case EINVARG: return "Invalid argument";
	case EINVADDR: return "Invalid address";
	case EINVINST: return "Invalid instruction";
	}
	return "Unknown error code";
}

int mips32r1_run(struct mips32r1_machine* m, u64 limit, int debug)
{
	int ret = 0;
	if (!m) {
		return ENULLPTR;
	}
	m->step = 0;
	m->limit = limit;
	m->debug = debug;
	while (!ret) {
		ret = cpu_step(m, 0);
	}
	if (ret) {
		fprintf(stderr, "Error: %s\n", mips32r1_strerror(ret));
	}
	return ret;
}
