#pragma once

/* Function return codes */
enum {
	EXCEPTION    =   1, /* CPU-handled exception */
	SUCCESS      =   0, /* No error */
	/* Errors */
	ENULLPTR     = -05, /* Null pointer dereference */
	EALLOC       = -01, /* Dynamic allocation failure */
	EBUFOV       = -02, /* Buffer overflow */
	EBADLOC      = -03, /* Bad location */
	EINTOV       = -04, /* Integer overflow */
	EEMPTY       = -11, /* Unexpected zero-length buffer */
	EFULL        = -12, /* Full buffer */
	ENYI         = -13, /* Not yet implemented */
	QUIT         = -16, /* Quitting */
	EINVARG      = -17, /* Invalid argument */
	EINVADDR     = -18, /* Invalid address */
	EINVINST     = -19, /* Invalid instruction */
	ENOTFOUND    = -20  /* Search pattern not found */
};

/* Return a human-readable explanation for an error code */
const char* zorafly_strerror(int ret);
