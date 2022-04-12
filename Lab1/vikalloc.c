// R. Jesse Chaney
// rchaney@pdx.edu

#include "vikalloc.h"

#define BLOCK_SIZE (sizeof(mem_block_t))

// Get to the user data within the give block.
#define BLOCK_DATA(__curr) (((void *) __curr) + (BLOCK_SIZE))

// Just making it easier to print some pointers.
#define PTR "0x%07lx"
#define PTR_T PTR "\t"

// I use this as the head of the heap map. This is also used in the function
// vikalloc_dump2() to display the contents of the heap map.
static mem_block_t *block_list_head = NULL;

// I use this to use this to point to the last block in the heap map. However,
// I dont' really make use of this for any really good reason.
//static mem_block_t *block_list_tail = NULL;

// I use these as the low water and high water pointer value for the heap
// that bevalloc is using. These are used in vikalloc_dump2().
static void *lower_mem_bound = NULL;
static void *upper_mem_bound = NULL;

// Sometimes we want a chatty allocator, like when debugging.
static uint8_t isVerbose = FALSE;

// This is where all the diagnostic output is sent (like all the stuff from
// vikalloc_dump2()). This is established with some gcc magic. You can alter
// it from the command line into another file.
static FILE *vikalloc_log_stream = NULL;

// The afore memtioned gcc magic.
static void init_streams(void) __attribute__((constructor));

// This is the variable that is used to determine how much space is requested
// from each call to sbrk(). Each call to sbrk() must be a multiple of this
// value.
static size_t min_sbrk_size = DEFAULT_SBRK_SIZE;

// The gcc magic implementation.
static void 
init_streams(void)
{
    vikalloc_log_stream = stderr;
}

// Allows us to chnage the multiple used for calls to sbrk().
size_t 
vikalloc_set_min(size_t size)
{
    if (0 == size) {
        // just return the current value
        return min_sbrk_size;
    }
    if (size < (BLOCK_SIZE + BLOCK_SIZE)) {
        // In the event that is is set to something silly.
        size = MAX(BLOCK_SIZE + BLOCK_SIZE, SILLY_SBRK_SIZE);
    }
    min_sbrk_size = size;
    return min_sbrk_size;
}

// To be chatty or not to be chatty...
void 
vikalloc_set_verbose(uint8_t verbosity)
{
    isVerbose = verbosity;
    if (isVerbose) {
        fprintf(vikalloc_log_stream, "Verbose enabled\n");
    }
}

// Redirect the diagnostic output into another file.
void 
vikalloc_set_log(FILE *stream)
{
    vikalloc_log_stream = stream;
}

// This is where the fun begins.
// You need to be able to split the first existing block that can support the
// the resuested size.
void *
vikalloc(size_t size)
{
	//void *str = NULL;
	mem_block_t *curr = NULL;
	mem_block_t *prev = NULL;

	if (isVerbose) {
		fprintf(vikalloc_log_stream, ">> %d: %s entry: size = %lu\n"
				, __LINE__, __FUNCTION__, size);
	}

	if (!size || size == 0) {
		return NULL;
	}

	if (block_list_head == NULL) {
		int i = 1;
		while (i * min_sbrk_size <= size + BLOCK_SIZE) {
			++i;
		}
		curr = sbrk(min_sbrk_size * i);
		curr->free = FALSE;
		curr->capacity = (min_sbrk_size * i) - BLOCK_SIZE;
		curr->size = size;
		curr->prev = curr->next = NULL;
		block_list_head = curr;
		lower_mem_bound = block_list_head;
		upper_mem_bound = lower_mem_bound + (0x400 * i);
	}

	else if (block_list_head != NULL) {
		int flag = 0;
		curr = block_list_head;
		// free block & split
		while (curr != NULL && flag == 0) {
			if (curr->free == TRUE && curr->capacity >= size) {
				curr->free = FALSE;
				curr->size = size;
				flag = 1;
			}
			else if (curr->capacity - curr->size >= size + BLOCK_SIZE) {
				prev = curr;
				curr = BLOCK_DATA(prev) + prev->size;
				curr->free = FALSE;
				curr->size = size;
				curr->capacity = prev->capacity - prev->size - BLOCK_SIZE;
				prev->capacity = prev->size;
				curr->next = prev->next;
				curr->prev = prev;
				prev->next = curr;
				if (curr->next) {
					curr->next->prev = curr;
				}
				flag = 1;
			}
			else {
				curr = curr->next;
			}
		}
		// allocate new memory
		if (flag == 0) {
			int i = 1;
			while (i * min_sbrk_size <= size + BLOCK_SIZE) {
				++i;
			}
			prev = block_list_head;
			while (prev->next != NULL) {
				prev = prev->next;
			}
			curr = sbrk(min_sbrk_size * i);
			curr->free = FALSE;
			curr->capacity = (min_sbrk_size * i) - BLOCK_SIZE;
			curr->size = size;
			curr->prev = prev;
			prev->next = curr;
			curr->next = NULL;
			upper_mem_bound += (0x400 * i);
		}
	}
	return BLOCK_DATA(curr);
}

// Free the block that contains the passed pointer. You need to coalesce adjacent
// free blocks.
void 
vikfree(void *ptr)
{
	mem_block_t *curr = block_list_head;
	mem_block_t *prev = NULL;
	mem_block_t *next = NULL;
	int flag = 0;
	while (curr != NULL && flag == 0) {
		if (BLOCK_DATA(curr) == ptr) {
			if (curr->free == TRUE) {
				if (isVerbose) {
					fprintf(vikalloc_log_stream, "Block is already free: ptr = " PTR "\n"
							, (long) (ptr - lower_mem_bound));
				}
				flag = 1;
			}
			else {
				curr->free = TRUE;
				curr->size = 0;
				flag = 1;
			}
		}
		else {
			curr = curr->next;
		}
	}
	if (!curr) {
		return;
	}
	if (curr->free && curr->prev && curr->prev->free) {
		prev = curr->prev;
		prev->capacity += curr->capacity + BLOCK_SIZE;
		if (curr->next) {
			curr->next->prev = curr->prev;

		}
		if (curr->prev) {
			curr->prev->next = curr->next;
		}
		curr->next = curr->prev = NULL;
		curr = prev;
	}
	if (curr->free && curr->next && curr->next->free) {
		next = curr->next;
		curr->capacity += next->capacity + BLOCK_SIZE;
		if (next->next) {
			next->next->prev = next->prev;
		}
		if (next->prev) {
			next->prev->next = next->next;
		}
		next->next = next->prev = NULL;
		next = curr;
	}

	return;
}

// Release the kraken, or at least all the vikalloc heap. This should leave
// everything as though it had never been allocated at all. A call to vikalloc
// that follows a call the vikalloc_reset starts compeletely from nothing.
void 
vikalloc_reset(void)
{
    brk(lower_mem_bound);
    block_list_head = NULL;
    lower_mem_bound = upper_mem_bound = NULL;
}

// Is like the regular calloc().
void *
vikcalloc(size_t nmemb, size_t size)
{
    void *ptr = NULL;

    if (!nmemb || !size) {
	    return NULL;
    }
    ptr = vikalloc(nmemb * size);
    memset(ptr, 0, nmemb * size);

    return ptr;
}

// Like realloc, but simpler.
// If the resuested new size does not fit into the current block, create 
// a new block and copy contents. If the requested new size does fit, just 
// adjust the size data member.
void *
vikrealloc(void *ptr, size_t size)
{
	mem_block_t *curr = block_list_head;
	void *new_ptr;
	int flag = 0;
	if (!size) {
		return NULL;
	}
	if (!ptr) {
		return vikalloc(size);
	}
	while (curr != NULL && flag == 0) {
		if (BLOCK_DATA(curr) == ptr) {
			if (curr->capacity >= size) {
				curr->size = size;
				if (curr->free) {
					curr->free = FALSE;
				}
				flag = 1;
			} else {
				new_ptr = vikalloc(size);
				memmove(new_ptr, ptr, size);
				vikfree(ptr);
				flag = 2;
			}
		} else {
			curr = curr->next;
		}
	}	
	if (flag == 2) {
		return new_ptr;
	}
	return ptr;

}

// Like the old strdup, but uses vikalloc().
void *
vikstrdup(const char *s)
{
    void *ptr = NULL;
    size_t size = strlen(s) + 1;
    if (s == NULL) {
	    return NULL;
    }
    ptr = vikalloc(size);
    if (ptr) {
	    memcpy(ptr, s, size);
    }
    return ptr;
}

// It is totaly gross to include C code like this. But, it pulls out a big
// chunk of code that you don't need to change. It needs access to the static
// variables defined in this module, so it either needs to be included, as
// here, as gross as it may be, or all the code directly in the module.
#include "vikalloc_dump.c"
