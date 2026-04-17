#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "types.h"
#include "error.h"
#include "mips32r1.h"
#include "fileutil.h"
#include "debug.h"
#include "mem.h"

void syntax(char* pidname)
{
	fprintf(stderr, "Syntax: %s <path to kernel> <path to ISR>\n",
		pidname);
	exit(1);
}

int main(int argc, char** argv)
{
	struct mips32r1_machine* m;
	int ret;
	char* program;
	long len;
	if (argc < 3) {
		syntax(argv[0]);
	}
	ret = mips32r1_new(&m);
	if (ret) {
		log_debug("failed to create new machine: %s",
			  mips32r1_strerror(ret));
		return ret;
	}
	ret = read_file(&program, &len, argv[2]);
	if (ret) {
		log_debug("failed to read file");
		return ret;
	}
	ret = mips32r1_load(m, mips32r1_interrupt1_vector, 0, program, len);
	free(program);
	if (ret) {
		log_debug("failed to load ISR1: %s",
			  mips32r1_strerror(ret));
		return ret;
	}
	ret = vmprotect(m->mem, mips32r1_interrupt1_vector, len, PROT_READ);
	if (ret) {
		log_debug("failed to protect kernel: %s",
			  mips32r1_strerror(ret));
		return ret;
	}
	ret = read_file(&program, &len, argv[1]);
	if (ret) {
		log_debug("failed to read file");
		return ret;
	}
	ret = mips32r1_load(m, 0xff000000, 0xff000000, program, len);
	free(program);
	if (ret) {
		log_debug("failed to load kernel: %s",
			  mips32r1_strerror(ret));
		return ret;
	}
	ret = vmprotect(m->mem, 0xff000000, len, PROT_READ);
	if (ret) {
		log_debug("failed to protect kernel: %s",
			  mips32r1_strerror(ret));
		return ret;
	}
	ret = mips32r1_run(m, 100, 1);
	mips32r1_free(m);
	log_debug("machine returned: %s",
		  mips32r1_strerror(ret));
	return ret;
}
