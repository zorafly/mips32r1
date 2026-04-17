#pragma once

int read_file(void* dst, long* dstlen, char* src);
int write_file(char* dst, long srclen, void* src);
long file_size(char* path);
