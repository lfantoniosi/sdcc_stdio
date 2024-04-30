#include <string.h>

#include "include/stdio.h"
#include "include/bdos.h"
#include "include/stdlib.h"


typedef struct header header_t;

struct header
{
	header_t *next; // Next block. Linked list of all blocks, terminated by pointer to end of heap (or to the byte beyond the end of the heap).
	header_t *next_free; // Next free block. Used in free blocks only. Overlaps with user data in non-free blocks. Linked list of free blocks, 0-terminated.
};

header_t *__sdcc_heap_free = NULL; // First free block, 0 if no free blocks.

extern void* __sdcc_heap_start;
extern void* __sdcc_heap_end; // Just beyond the end of the heap. Must be higher in memory than _sdcc_heap_start.

void __sdcc_heap_init(void)
{
	__sdcc_heap_free = (header_t*)__sdcc_heap_start;
	__sdcc_heap_free->next = (header_t*)__sdcc_heap_end;
	__sdcc_heap_free->next_free = 0;
}

void *malloc(size_t size)
{
	header_t *h;
	header_t **f;

	if(!__sdcc_heap_free)
		__sdcc_heap_init();

	if(!size || size + offsetof(struct header, next_free) < size)
	{
		return(0);
	}
		
	size += offsetof(struct header, next_free);
	if(size < sizeof(struct header)) // Requiring a minimum size makes it easier to implement free(), and avoid memory leaks.
		size = sizeof(struct header);

	for(h = __sdcc_heap_free, f = &__sdcc_heap_free; h; f = &(h->next_free), h = h->next_free)
	{
		size_t blocksize = (char *)(h->next) - (char *)h;
		if(blocksize >= size) // Found free block of sufficient size.
		{
			if(blocksize >= size + sizeof(struct header)) // It is worth creating a new free block
			{
				header_t *const newheader = (header_t *const)((char *)h + size);
				newheader->next = h->next;
				newheader->next_free = h->next_free;
				*f = newheader;
				h->next = newheader;
			}
			else
				*f = h->next_free;
			return(&(h->next_free));
		}
	}
	return(0);
}


void free(void *ptr)
{
	header_t *h, *next_free, *prev_free;
	header_t **f;
	
	if(!ptr)
		return;

	prev_free = 0;
	for(h = __sdcc_heap_free, f = &__sdcc_heap_free; h && h < ptr; prev_free = h, f = &(h->next_free), h = h->next_free); // Find adjacent blocks in free list
	next_free = h;

	h = (void *)((char *)(ptr) - offsetof(struct header, next_free));

	// Insert into free list.
	h->next_free = next_free;
	*f = h;

	if(next_free == h->next) // Merge with next block
	{
		h->next_free = h->next->next_free;
		h->next = h->next->next;
	}

	if (prev_free && prev_free->next == h) // Merge with previous block
	{
		prev_free->next = h->next;
		prev_free->next_free = h->next_free;
	}
}



