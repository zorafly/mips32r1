#define _POSIX_C_SOURCE 200800L
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "types.h"
#include "error.h"
#include "mips32r1.h"

int log_debug(const char* format, ...)
{
	char buf[4096] = {0};
	char datetime[64] = {0};
	va_list ap;
	time_t t0;
	struct tm* t1;
	struct timespec ts = {0};
       
	t0 = time(NULL);
	t1 = localtime(&t0);
	clock_gettime(CLOCK_MONOTONIC, &ts);
	strftime(datetime, 64, "%b %d %H:%M:%S", t1);
	
	snprintf(buf, 4096, "[%s:%li] ==> %s\n",
		 datetime, ts.tv_nsec / 1000, format);
	
	va_start(ap, format);
	vfprintf(stderr, buf, ap);
	va_end(ap);
	return SUCCESS;
}
