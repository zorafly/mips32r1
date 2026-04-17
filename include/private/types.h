#pragma once

#define U8_MAX 255
#define U16_MAX 65535
#define U32_MAX 4294967295
#define U64_MAX 18446744073709551615
#define I8_MAX  127
#define I16_MAX 32767
#define I32_MAX 2147483647
#define I64_MAX 9223372036854775808
#define I8_MIN  (-I8_MAX)
#define I16_MIN (-I16_MAX)
#define I32_MIN (-I32_MAX)
#define I64_MIN (-I64_MAX)

/* Integer types */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef __uint128_t u128;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef __int128_t i128;

/* Bitfields */
typedef u64 bits;

/* Vector types */
typedef union {
	u8 flat[16];
	double raw[2];
	struct {
		double x;
		double y;
	};
} dvec2;

typedef union {
	u8 flat[24];
	double raw[3];
	struct {
		double x;
		double y;
		double z;
	};
} dvec3;

typedef union {
	u8 flat[32];
	double raw[4];
	struct {
		double x;
		double y;
		double z;
		double w;
	};
} dvec4;

typedef union {
	u8 flat[16];
	u64 raw[2];
	struct {
		u64 x;
		u64 y;
	};
} uvec2;

typedef union {
	u8 flat[24];
	u64 raw[3];
	struct {
		u64 x;
		u64 y;
		u64 z;
	};
} uvec3;

typedef union {
	u8 flat[32];
	u64 raw[4];
	struct {
		u64 x;
		u64 y;
		u64 z;
		u64 w;
	};
} uvec4;

