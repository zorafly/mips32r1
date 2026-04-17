#pragma once

#define INSTRUCTION(_f)							\
	static int do_##_f(struct mips32r1_machine* m,			\
			   struct mips32r1_generic* i,			\
			   int delay)
typedef int (*executor)(struct mips32r1_machine*, struct mips32r1_generic* i,
			int delay);
int cpu_step(struct mips32r1_machine* m, int delay);
int cpu_handle_exception(struct mips32r1_machine* m, int delay);
int cpu_get_cp0reg(struct mips32r1_machine* m, u32** out, u8 reg, u8 sel);
