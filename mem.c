#include "sjip.h"
#include "mem.h"

u32	HEAP[HEAP_SIZE / sizeof(u32)];

void alloc_test(void)
{
	u8 *p[5];

	p[0] = mem_alloc(10);
	p[1] = mem_alloc(99);
	p[2] = mem_alloc(2000);

	mem_free(p[1]);
	mem_free(p[0]);
	p[0] = mem_alloc(104);
	/**/
	mem_free(p[2]);
	mem_free(p[0]);


	p[3] = mem_alloc(2104);
	mem_free(p[3]);
}


void heap_init(void)
{
	mem_header	*heap_header;

	heap_header = (mem_header*)HEAP;

	heap_header->type = M_IDLE;
	heap_header->size = HEAP_SIZE - sizeof(mem_header);
	heap_header->prev = NULL;
	heap_header->next = NULL;

	MEM_DBP("heap_init\r\n");
	MEM_DBP("sizeof(mem_header):%d\r\n", sizeof(mem_header));
}


/*

*/
void* mem_alloc(u32 alloc_size)
{
	mem_header	*mem_block;
	mem_header	*remain;
	mem_header	*next_block;

	static u8	init = 0;

	if(init == 0)
	{
		init = 1;
		heap_init();
	}

	mem_block = (mem_header*)HEAP;
	alloc_size = (alloc_size + 0x03) & 0xFFFFFFFC;	// ��֤�����ڴ�4�ֽڶ���

	MEM_DBP("alloc_size:%d\r\n", alloc_size);

	MEM_DBP("block_list:\r\n");

	do	// �����ڴ��
	{
		MEM_DBP("%d:%d %d\r\n", (u32)mem_block - (u32)HEAP, mem_block->size, mem_block->type);

		// �������� ��Ϊ����
		if((mem_block->size >= alloc_size) && (mem_block->type == M_IDLE))
		{
			break;
		}

		mem_block = mem_block->next;
	}
	while(mem_block != NULL);


	// ò�Ʋ�������  ʹ��ǰ����кϲ���free�㷨 ���ܲ�������������ڵĿ����ڴ�� ��֤��
	if(mem_block == NULL)	// δ�ҵ��㹻��Ŀ����ڴ�� ����ѿռ� �ϲ����������ڴ��
	{
		mem_block = (mem_header*)HEAP;

		do	// �����ڴ��
		{
			if(mem_block->type == M_IDLE)	// ��ǰ����
			{
				next_block = mem_block->next;

				while((mem_block->next != NULL) && (next_block->type == M_IDLE))	// ��һ�ڴ��ǿ��ҿ���
				{
					next_block = mem_block->next;
					mem_block->size += next_block->size + sizeof(mem_header);
					next_block->next->prev = mem_block;
					mem_block->next = next_block->next;
				}

				if(mem_block->size >= alloc_size)
				{
					break;
				}
			}
			
			mem_block = mem_block->next;	// �����һ�ڴ��
		}
		while(mem_block != NULL);
	}

	if(mem_block != NULL)	// �ҵ��㹻��Ŀ����ڴ�� ���Ϊ����
	{
		mem_block->type = M_BUSY;

		// ԭ�ڴ�黹�㹻������һ�ڴ��
		if(mem_block->size >= (alloc_size + (sizeof(mem_header) + sizeof(u32))))
		{
			remain = (mem_header*)((u32)mem_block + sizeof(mem_header) + alloc_size);
			remain->type = M_IDLE;
			remain->size = mem_block->size - alloc_size - sizeof(mem_header);
			remain->prev = mem_block;
			remain->next = mem_block->next;

			if(remain->next != NULL)
			{
				remain->next->prev = remain;
			}

			mem_block->size = alloc_size;
			mem_block->next = remain;
		}

		MEM_DBP("New Block Start At:%d\r\n\r\n", (u32)mem_block + sizeof(mem_header) - (u32)HEAP);
		return (void*)((u32)mem_block + sizeof(mem_header));
	}

	return NULL;
}

void mem_free(void* p)
{
	mem_header* cur_block;
	mem_header* next_block;

	cur_block = (mem_header*)((u32)p - sizeof(mem_header));

	if(cur_block->type == M_BUSY)
	{
		cur_block->type = M_IDLE;

		// ��һ�ڴ��ǿ� ����
		if(cur_block->next != NULL)
		{
			MEM_DBP("Next ");
			next_block = cur_block->next;

			if(next_block->type == M_IDLE)
			{
				MEM_DBP("Idle\r\n");
				// ���ߺϲ�
				cur_block->size += next_block->size + sizeof(mem_header);
				cur_block->next = next_block->next;

				if(next_block->next != NULL)
				{
					next_block->next->prev = cur_block;
				}
			}
			else
			{
				MEM_DBP("Busy\r\n");
			}
		}

		// ��һ�ڴ��ǿ� ����
		if(cur_block->prev != NULL)
		{
			MEM_DBP("Prev ");

			if(cur_block->prev->type == M_IDLE)
			{
				MEM_DBP("Idle\r\n");
				// ���ߺϲ�
				cur_block->prev->size += cur_block->size + sizeof(mem_header);
				cur_block->prev->next = cur_block->next;

				if(cur_block->next != NULL)
				{
					cur_block->next->prev = cur_block->prev;
				}
			}
			else
			{
				MEM_DBP("Busy\r\n");
			}
		}

		MEM_DBP("\r\n");

	}
}



void mem_copy(u8* dst, const u8* src, u32 len)
{
	u32 i;

	for(i = 0; i < len; i++)
	{
		dst[i] = src[i];
	}
}

u8 mem_comp(const u8* tar, const u8* src, u32 len)
{
	u32 i;

	for(i = 0; i < len; i++)
	{
		if(tar[i] != src[i])
		{
			return MEM_DIFF;
		}
	}

	return MEM_EQU;
}

void mem_setbytes(u8 *tar, u8 val, u32 len)
{
    u32 i;
    
    for(i=0; i<len; i++)
    {
        tar[i]=val;
    }
}

void mem_clrbytes(u8 *tar, u32 len)
{
    u32 i;
    
    for(i=0; i<len; i++)
    {
        tar[i]=0;
    }
}
