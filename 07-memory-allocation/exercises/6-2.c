#define _BSD_SOURCE

/*
	Come back to this later

*/


const unsigned long PAGE_SIZE = sysconf(_SC_PAGESIZE);


typedef struct MEMORY_NODE {

	void *prev;
	void *next;
	void *mem;

} MEMORY_NODE;


typedef struct FREE_NODE {
	
	size_t length;
	void *prev;
	void *next;
	void *mem;

} FREE_NODE;


MEMORY_NODE *MEMORY_LIST = NULL;
FREE_NODE *FREE_LIST = NULL;



void *fetch_from_free(size_t size)
{
	FREE_NODE *node = FREE_LIST;

	// very simple first fit algorithm
	while (node->prev) {
		if (node->length >= size) {
			return node->mem;
		}

		node = node->prev;
	}

	// no match in the free list
	return NULL;
}


size_t get_chunk_size(size_t size)
{
	
}


void *_malloc(size_t size)
{
	void *mem = NULL;

	if (!size || size < 0) {
		return NULL;
	}

	// see if you can get that memory from free list
	if (FREE_LIST) {
		mem = fetch_from_free(size);

		if (mem) {
			return mem;
		}
	}

	// guess not
	// get the size of memory to be allocated
	// based on the closest multiple of the virtual memory page size of the system
	return sbrk(get_chunk_size(size));
}


void *_realloc(size_t size)
{
	
}

void free(void *ptr)
{
	MEMORY_LIST *mem_node = MEMORY_LIST;

	if (!mem_node) {
		return -1;
	}

	while (mem_node->prev) {

		if (ptr == mem_node->mem) {

			if (!mem_node->next) {

				sbrk(-(*(((int *)mem_node->mem)-1)));
				return 0;

			} else {

				int size = *(((int *)mem_node->mem)-1);
				

			}


		}

		mem_node = mem_node->prev;
	}

	return -1;
}



int main(int argc, char *argv[])
{

	exit(EXIT_SUCCESS);
}
