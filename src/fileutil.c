#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fileutil.h"

int read_file(void* dst, long* dstlen, char* src)
{
	FILE* srcfp;
	long len;
	void** _dst = (void**)dst;
	if (!dstlen) {
		dstlen = &len;
	}
	if (!dst || !dstlen || !src) {
		return 1;
	}
	srcfp = fopen(src, "r");
	if (!srcfp) {
		return 1;
	}
	if (fseek(srcfp, 0, SEEK_END)) {
		return fclose(srcfp);
	}
	*dstlen = ftell(srcfp);
	if (*dstlen == -1) {
		return fclose(srcfp);
	}
	if (fseek(srcfp, 0, SEEK_SET)) {
		return fclose(srcfp);
	}
	*_dst = malloc(*dstlen+1);
	if (!*_dst) {		
		return fclose(srcfp);
	}
	if (fread(*_dst, *dstlen, 1, srcfp) != 1) {
		free(*_dst);
		return fclose(srcfp);
	}
	if (ferror(srcfp)) {
		free(*_dst);
		return fclose(srcfp);
	}
	if (fclose(srcfp)) {
		free(*_dst);
		return 1;
	}
	((uint8_t*)(*_dst))[*dstlen] = 0;
	return 0;
}

int write_file(char* dst, long srclen, void* src)
{
	FILE* dstfp;
	dstfp = fopen(dst, "w");
	if (!dstfp) {
		return errno;
	}
	if (fwrite(src, srclen, 1, dstfp) != 1) {
		return fclose(dstfp);
	}
	if (ferror(dstfp)) {
		return fclose(dstfp);
	}
	return fclose(dstfp);
}

long file_size(char* path) {
	FILE* fp;
	long len;
	if (!path) {
		return -1;
	}
	fp = fopen(path, "r");
	if (!fp) {
		return -1;
	}
	if (fseek(fp, 0, SEEK_END) == -1) {
		fclose(fp);
		return -1;
	}
	len = ftell(fp);
	if (len == -1) {
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return len;
}

