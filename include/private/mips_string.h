#pragma once

size_t string_length(char* in, size_t inlen);
int string_copy(char** out, size_t* outlen, char* in, size_t inlen);
int string_findchar(size_t* out, char* in, size_t inlen, char c);
