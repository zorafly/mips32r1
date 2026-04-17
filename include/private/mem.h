#pragma once

#define MMAP_FLAGS (MAP_PRIVATE|MAP_POPULATE|MAP_ANONYMOUS)
/* Copy directions as passed to vmemcpy() */
enum {
	TO_HOST = 0, /* Copy from virtual to physical memory */
	TO_VM   = 1  /* Copy from physical to virtual memory */
};
enum {
	FROM_VM = 0,
	FROM_HOST = 1
};

struct vram {
	size_t aslen;       /* Size of a single address space in bytes */
	unsigned pagelen;   /* Size of a single page in bytes */
	unsigned npages;    /* Number of pages per address space */
	unsigned maxalloc;  /* Maximum number of pages to allocate */
	unsigned nalloc;    /* Current number of allocated pages */
	unsigned nbuckets;  /* Number of buckets */
	unsigned bucketlen; /* Number of pages per bucket */
/******* Example layout of three-level address translation table ******

                        u8*** ram [npages == 2, nbuckets = 2]
                               |
 		u8** ram[0]----------u8** ram[1] :::: Bucket
		          |               |
	    u8* ram[0][0]---...          ...     :::: Page
                     |                    |
       u8 ram[0][0][0]---...             ...     :::: Offset

The top level of the table is empty for a new VM. Frames are allocated 
on-demand, during program runtime. Frames may not be freed. 
******/
	u8*** ram;
};

int vram_new(struct vram** out, unsigned width, unsigned pagelen,
	     unsigned maxalloc, unsigned buckets);
void vram_free(struct vram* in);
int vmemcpy(struct vram* mem, u64 virt, void* phys1, size_t len, u8 dir);
int vmprotect(struct vram* mem, u64 virt, size_t len, int prot);

