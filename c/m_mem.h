/*
 * LCDK-specific memory allocation functions
 * Use these instead of malloc, free, etc! e.g. use m_malloc and m_free
 * iff running code on the LCDK
 */

#ifndef MMEM
#define MMEM
#include "stdlib.h" 
#include "stdio.h"
#define MAX_SEG 5000
#define STATIC_MEM_SIZE 2*1024*1024
#define QUEUE_SIZE 12*1024*1024
#define INF 0xffffffff
#define BLOCK_SIZE 24

// If not using the LCDK, just let m_ functions be calls to their stdlib counterparts
#ifndef LCDK_BUILD

#include <stdlib.h>

void mem_init()
{
	return;
}

void* m_malloc(unsigned int size)
{
	return malloc((size_t)size);
}

void m_free(void* p)
{
	free(p);
}

#else
// LCDK build memory management functions
typedef struct seg
{
	void* left;
	void* right;
} seg_t;

seg_t pointers[MAX_SEG];

char m_mem[QUEUE_SIZE];
#pragma DATA_SECTION(m_mem,".EXT_RAM")
char s_mem[STATIC_MEM_SIZE];
#pragma DATA_SECTION(s_mem,".EXT_RAM")
unsigned char mem_count = 0;
unsigned int upper_bound_m = 0;
unsigned int upper_bound_s = 0;
void* sp;

void mem_init()
{
	int i;
	sp = s_mem;
	
	for(i = 1;i < MAX_SEG;i ++)
	{
		pointers[i].left = 0;
		pointers[i].right = 0;
	}
	if(m_mem == NULL || s_mem == NULL)
	{
		perror("init fail");
		exit(1);
	}
	pointers[0].left = m_mem;
	pointers[0].right = m_mem;
	mem_count ++;
}


void sort(seg_t* table)
{
	int i;
	int j;
	seg_t temp;
	for(i = 0;i < mem_count - 1;i ++)
	{
		for(j = 0;j < mem_count - 1 - i;j ++)
		{
			if(table[j].left > table[j + 1].left)
			{
				temp.left =  table[j + 1].left;
				temp.right =  table[j + 1].right;
				table[j + 1].left = table[j].left;
				table[j + 1].right = table[j].right;
				table[j].left = temp.left;
				table[j].right = temp.right;
			}
		}	
	}
}

void* m_malloc(unsigned int size)
{
	int i;
	if(mem_count >= MAX_SEG)
	{
		perror("no more segment");
		return NULL;
	}
	
	if(mem_count == 0)
	{
		if(size > QUEUE_SIZE)
		{
			perror("no enough mem");
			return NULL;
		}
		else
		{
			pointers[0].left = m_mem;
			pointers[0].right = m_mem + size - 1;
			//printf("Memory Count = %d\n", mem_count);
			//printf("Memory of size = %d Bytes is allocated address from %p to %p.\n", size, pointers[mem_count].left, pointers[mem_count].right);

			mem_count ++;
			upper_bound_m += size;

			return m_mem;
		}
	}

	sort(pointers);
	
	for(i = 0;i < mem_count - 1;i ++)
	{
		if((unsigned int)pointers[i + 1].left - (unsigned int)pointers[i].right > size)
		{
			pointers[mem_count].left = (void*)((unsigned int)pointers[i].right + 1);
			pointers[mem_count].right = (void*)((unsigned int)pointers[mem_count].left + size - 1);
			mem_count ++;
			return  pointers[mem_count - 1].left;
		}
	}
	if(size > QUEUE_SIZE  - ((unsigned int)pointers[i].right - (unsigned int)m_mem))
	{
		perror("no enough memory");
		return NULL;
	}
	else
	{
		pointers[mem_count].left = (void*)((unsigned int)(pointers[mem_count - 1].right) + 1);
		pointers[mem_count].right = (void*)((unsigned int)pointers[mem_count].left + size - 1);
		//printf("Memory of size = %d Bytes is allocated address from %p to %p.\n", size, pointers[mem_count].left, pointers[mem_count].right);

		mem_count ++;
		if((unsigned int)pointers[mem_count].right - (unsigned int)m_mem + 1 > (unsigned int)upper_bound_m)
		{
			upper_bound_m = (unsigned int)pointers[mem_count - 1].right - (unsigned int)m_mem + 1;
		}


		return  pointers[mem_count - 1].left;
	}
}

void m_free(void* p)
{
	int i;
	if(mem_count <= 1)
	{
		return;
	}
	for(i = 0;i < mem_count;i ++)
	{
		if(pointers[i].left == p)
		{
			pointers[i].left = (void*)INF;
			pointers[i].right = (void*)INF;
			break;
		}
	}
	sort(pointers);
	pointers[mem_count - 1].left = 0;
	pointers[mem_count - 1].right = 0;
	mem_count --;
}

void* s_malloc(unsigned int size)
{
	if((unsigned int)sp + BLOCK_SIZE > (unsigned int)s_mem + STATIC_MEM_SIZE - 1)
	{
		return NULL;
	}
	sp = (void*)((unsigned int)sp + BLOCK_SIZE);
	if((unsigned int)sp - (unsigned int)s_mem > (unsigned int)upper_bound_s)
	{
		upper_bound_s = (unsigned int)sp - (unsigned int)s_mem;
	}
	return (void*)((unsigned int)sp - BLOCK_SIZE);
}

void s_free(void* p)
{
	if((unsigned int)sp - BLOCK_SIZE >= (unsigned int)s_mem)
		sp =  (void*)((unsigned int)sp - BLOCK_SIZE);
}

void m_stat()
{
	if(upper_bound_m / 1024 / 1024 != 0)
	{
		printf("The process used at most %u MB external memory.\n", upper_bound_m / 1024 / 1024);
	}
	else if(upper_bound_m / 1024 != 0)
	{
		printf("The process used at most %u KB external memory.\n", upper_bound_m / 1024);
	}
	else
	{
		printf("The process used at most %u BYTE external memory.\n", upper_bound_m);
	}
	
	if(upper_bound_s / 1024 / 1024 != 0)
	{
		printf("The process used at most %u MB external stack.\n", upper_bound_s / 1024 / 1024);
	}
	else if(upper_bound_s / 1024 != 0)
	{
		printf("The process used at most %u KB external stack.\n", upper_bound_s / 1024);
	}
	else
	{
		printf("The process used at most %u BYTE external stack.\n", upper_bound_s);
	}
}

#endif

#endif
