#ifndef	__MEN_H
#define	__MEN_H

#define HEAP_SIZE	(8*1024)

#define	M_IDLE	0
#define M_BUSY	1

#define MEM_DIFF	0
#define MEM_EQU		1


#undef	NULL
#define NULL	(void*)0

typedef struct mem
{
	u32			type;
	u32			size;
	struct mem* prev;
	struct mem*	next;
}mem_header;

void* mem_alloc(u32 size);
void mem_free(void* p);
void mem_copy(u8* dst,const u8* src, u32 len);
u8 mem_comp(const u8* tar, const u8* src, u32 len);
void mem_setbytes(u8 *tar, u8 val, u32 len);
void mem_clrbytes(u8 *tar, u32 len);

#endif
