#ifndef   	_MEM_ALLOC_H_
#define   	_MEM_ALLOC_H_

/* Structure declaration for a free block */
typedef struct free_block{
  int size; 
  struct free_block *next;
  struct free_block *before;
} free_block_s, *free_block_t; 

/* Structure declaration for an occupied block */
typedef struct busy_block{
  int size;
  struct busy_block *next;
} busy_block_s, *busy_block_t; 

/* Allocator functions, to be implemented in mem_alloc.c */
void memory_init(void); 
char *memory_alloc(int size); 
void memory_free(char *p);

/* Logging functions */
void print_info(void); 
void print_alloc_info(char *addr, int size); 
void print_free_info(char *addr); 
void print_free_blocks(void); 
char *heap_base(void);
int validAdress(char *p);
free_block_t getPreviousFreeBlock(busy_block_t block);

#include <stdlib.h>
//void *malloc(size_t size); 
//void free(void *p); 
//void *realloc(void *ptr, size_t size); 


#endif 	    /* !_MEM_ALLOC_H_ */
