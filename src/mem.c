#define _GNU_SOURCE 1
#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "types.h"
#include "error.h"
#include "mem.h"
#include "debug.h"

/* Resolve the physical address of a linear virtual address */
static void* linear_addr(struct vram* mem, int* out, u64 in)
{
	u32 bucket;
	u32 frame;
	unsigned off;
	bucket = in / (mem->nbuckets * mem->bucketlen);
	frame = (in - (mem->bucketlen * mem->pagelen * bucket)) / mem->pagelen;
	if (frame >= mem->npages || bucket >= mem->nbuckets) {
		log_debug("out-of-bounds frame (%u) or bucket (%u)",
			  frame, bucket);
		*out = EINVADDR;
		return NULL;
	}
	/* Allocate the bucket if needed */
	if (!mem->ram[bucket]) {
		if (mem->nalloc >= mem->maxalloc) {
			goto ealloc;
		}
		mem->ram[bucket] = mmap(NULL, mem->pagelen * mem->bucketlen,
					PROT_READ|PROT_WRITE, MMAP_FLAGS,
					-1, 0);
		if (mem->ram[bucket] == MAP_FAILED) {
			log_debug("bucket map failed: %s", strerror(errno));
		}
		memset(mem->ram[bucket], 0, mem->bucketlen * mem->pagelen);
		log_debug("allocated bucket %u at P:%p",
			  bucket, mem->ram[bucket]);
	}
	/* Allocate the frame if needed */
	if (!mem->ram[bucket][frame]) {
		mem->ram[bucket][frame] = mmap(NULL, mem->pagelen,
					       PROT_READ|PROT_WRITE,
					       MMAP_FLAGS,
					       -1, 0);
		if (mem->ram[bucket][frame] == MAP_FAILED) {
			log_debug("page map  failed: %s", strerror(errno));
			goto ealloc;
		}
		memset(mem->ram[bucket][frame], 255, mem->pagelen);
		mem->nalloc++;
		log_debug("allocated frame %u at V:%08x P:%p",
			  frame,
			  ((mem->bucketlen * bucket) + frame) * mem->pagelen, 
			  mem->ram[bucket][frame]);
	}
	/* Calculate offset into frame */
	off = in % mem->pagelen;
	*out = SUCCESS;
	return &mem->ram[bucket][frame][off];
ealloc:
	*out = EALLOC;
	return NULL;
}

static void* phys_addr(struct vram* mem, int* out, u64 in)
{
	if (!out) {
		return NULL;
	}
	if (!mem) {
		*out = ENULLPTR;
		return NULL;
	}
	/* Verify address is within valid range */
	if (in > mem->aslen - 1) {
		*out = EINVADDR;
		return NULL;
	}
	/* Allocate the page table if needed */
	if (!mem->ram) {
		mem->ram = malloc(sizeof(u8**)*mem->nbuckets);
		if (!mem->ram) {
			*out = EALLOC;
			return NULL;
		}
		memset(mem->ram, 0, sizeof(u8**)*mem->nbuckets);
	}
	/* Resolve the address */
	return linear_addr(mem, out, in);
}

static int memcpy_crosspage(struct vram* mem, u64 virt, void* phys, size_t len,
			    u8 dir)
{
	int ret;
	unsigned w;     /* Number of bytes to write in one loop iteration */
	u8* dst = NULL;
	u8* src = NULL;
	unsigned t = 0; /* Total bytes written */

	/* When copying to virtual memory, the source address never needs to
	   change. */
	if (dir == TO_VM) {
		src = phys;
	}
	/* The inverse condition is true when copying to physical memory */
	else {
		dst = phys;
	}
	while (t < len) {
		if (dir == TO_VM) {
			dst = phys_addr(mem, &ret, virt);
			if (ret) {
				return ret;				
			}
			if (!dst) {
				return EINVADDR;
			}
		}
		/* When copying from virtual memory, the source address must
		   be resolved for each page. */
		else {
			src = phys_addr(mem, &ret, virt);
			if (ret) {
				return ret;
			}
			if (!src) {
				return EINVADDR;
			}
		}
		/* Calculate the number of bytes which can fit into the area
		   starting at virt */
		w = (mem->pagelen * ((virt / mem->pagelen) + 1)) - virt;

		/* when w is larger than the total amount of requested copy
		   bytes, reduce it by the amount of bytes already copied. */
		if (w > len - t) {
			w = len - t;
		}
		/* When copying to virtual memory, we index into the source
		   address as the destination address is resolved on each
		   iteration of the loop */
		if (dir == TO_VM) {
			memcpy(dst, &src[t], w);
		}
		/* We index into the destination address here for the opposite
		   of the reason explained above */
		else {
			memcpy(&dst[t], src, w);
		}
		virt += w;
		t += w;
	}
	return SUCCESS;
}

int vram_new(struct vram** out, unsigned width, unsigned pagelen,
	     unsigned maxalloc, unsigned buckets)
{
	struct vram* p;
	if (!out) {
		return ENULLPTR;
	}
	if (!width || !pagelen || !maxalloc) {
		return EINVARG;
	}
	p = malloc(sizeof(struct vram));
	if (!p) {
		return EALLOC;
	}
	memset(p, 0, sizeof(struct vram));
	p->aslen = 1ULL << width;
	p->pagelen = pagelen;
	p->maxalloc = maxalloc;
	p->npages = p->aslen / p->pagelen;
	p->nbuckets = buckets;
	p->bucketlen = p->npages / p->nbuckets;
	*out = p;
	return SUCCESS;
}

void vram_free(struct vram* in)
{
	unsigned i;
	unsigned j;
	if (!in) {
		return;
	}
	if (!in->ram) {
		goto fini;
	}
	for (i = 0; i < in->nbuckets; i++) {
		if (in->ram[i]) {
			for (j = 0; j < in->bucketlen; j++) {
				if (in->ram[i][j]) {
					munmap(in->ram[i][j], in->pagelen);
				}
			}
			munmap(in->ram[i], in->bucketlen * in->pagelen);
		}
	}
	free(in->ram);
fini:
	free(in);
	return;
}

int vmemcpy(struct vram* mem, u64 virt, void* phys1, size_t len, u8 dir)
{
	int ret;
	u8* phys2;
	unsigned i1;
	unsigned i2;
	if (!mem || !phys1) {
		return ENULLPTR;
	}
	if (virt+len > mem->aslen-1) {
		fprintf(stderr, "vram overflow on address: %lx\n", virt+len);
		return EINVADDR;
	}
	/* Calculate the highest and lowest page numbers responsible for
	   the requested virtual area. If they are the same, a copy is
	   fast and simple. Else, we hand off the job to memcpy_crosspage */
	i1 = virt / mem->pagelen;
	i2 = (virt + len) / mem->pagelen;
	if (i1 == i2) {
		phys2 = phys_addr(mem, &ret, virt);
		if (ret) {
			return ret;
		}
		if (!phys2) {
			return EINVADDR;
		}
		if (dir) {
			memcpy(phys2, phys1, len);
		}
		else {
			memcpy(phys1, phys2, len);
		}
		return SUCCESS;
	}
	return memcpy_crosspage(mem, virt, phys1, len, dir);
}

int vmprotect(struct vram* mem, u64 virt, size_t len, int prot)
{
	int ret;
	unsigned startpage;
	unsigned endpage;
	unsigned i;
	void* page;
	if (!mem) {
		return ENULLPTR;
	}
	if (!len) {
		return EEMPTY;
	}
	if (virt+len > mem->aslen-1) {
		log_debug("vram overflow on address: %lx", virt+len);
		return EINVADDR;
	}
	startpage = virt / mem->pagelen;
	endpage = (virt + len) / mem->pagelen;
	for (i = startpage; i < endpage; i++) {
		/* Prefault the page by resolving its base address */
		page = phys_addr(mem, &ret, i * mem->pagelen);
		if (ret) {
			return ret;
		}
		if (!page) {
			return ENULLPTR;
		}
		/* Protect the page */
		ret = mprotect(page, mem->pagelen, prot);
		if (ret) {
			log_debug("mprotect %p, %u failed: %s",
				  mem->ram[i], mem->pagelen, strerror(errno));
			return errno;
		}
	}
	return SUCCESS;
}
