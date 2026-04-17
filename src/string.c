#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"

size_t string_length(char* in, size_t inlen)
{
	size_t i;
	if (!in) {
		return 0;
	}
	i = 0;
	while (i < inlen) {
		if (!in[i]) {
			return i;
		}
		i++;
	}
	return i;
}

int string_copy(char** out, size_t* outlen, char* in, size_t inlen)
{
	char* s;
	if (!out || !in) {
		return ENULLPTR;
	}
	if (!inlen) {
		return EEMPTY;
	}
	*outlen = string_length(in, inlen);
	if (!*outlen) {
		return EEMPTY;
	}
	inlen = *outlen;
	s = malloc(inlen+1);
	if (!s) {
		return EALLOC;
	}
	memcpy(s, in, inlen);
	s[inlen] = 0;
	*out = s;
	return SUCCESS;
}

int string_findchar(size_t* out, char* in, size_t inlen, char c)
{
	size_t i;
	if (!in || !inlen || !out) {
		return ENULLPTR;
	}
	for (i = 0; i < inlen; i++) {
		if (in[i] == c) {
			*out = i;
			return i;
		}
	}
	return ENOTFOUND;
}
