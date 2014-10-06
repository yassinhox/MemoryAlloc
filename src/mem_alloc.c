	#include "mem_alloc.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* memory */
char memory[MEMORY_SIZE]; 


/* Pointer to the first free block in the memory */
free_block_t first_free; 
busy_block_t first_occupied;   // We didn't have this in the definition but there is no damage using it as it is only just a pointer but haven't been yet initialized


#define ULONG(x)((long unsigned int)(x))
#define max(x,y) (x>y?x:y)


void memory_init(void)
{
	// Using this method I found out the size of the free block and the busy block :  printf("%lu \n %lu " , sizeof(free_block_s),sizeof(busy_block_s));
	// Size of the free block = 16 , Size of the busy block = 4  
	// If you look at the definition of the malloc , you will realize that we can't use it here ! 
	
	
	first_free = (free_block_t)memory ;
	first_free->size=MEMORY_SIZE;
	first_free->next=NULL;
	print_free_blocks();
	// first_free->before=NULL;	
	
}

 char *memory_alloc(int size)  
{
	int temp_size=0;
	 size=size+sizeof(busy_block_s);  
	 
	  printf("allocating memory of size %d \n ", size);
	 
	 if (size>MEMORY_SIZE)  // Requesting a big size
	 {
		 printf("Size is greater than the memory , can't allocate \n");
	 return NULL;
	 }  
	 	 
	 if ( first_free == (free_block_t) memory ) // We only have one free block , which is the initialization case 
	 {
	       
	       printf("First case : All the memory is empty \n");
	
		  busy_block_t block = (busy_block_t) first_free;
		  temp_size= first_free->size;
		  block->size =size;
		  
		  free_block_t node = (free_block_t) ((char*) block + size);
		  
		   // printf("Busy block size : %d , first free size : %d , node size %d \n" ,block->size,first_free->size,node->size); 
		  node->size = temp_size - block->size; 
		
		  node->next = NULL; 
		  first_free=node;
		
		printf("Busy block size : %d , first free size : %d , node size %d \n" ,block->size,first_free->size,node->size);   
		  printf("Allocation Succeeded address of busy %p address of empty %p \n",(char*)block,(char*)node);
		  
		  printf("First free at address %p \n and it's size %d \n",(char*)first_free , node->size);
		  	return ((char*)block);

 }
		 
	
	 free_block_t temp =first_free; 
	 free_block_t temp_before =first_free;
	 
	 
	int i=0;
	 
	do
	{
		
		if(size <= (temp->size))
		{
				printf("found the required space at address %p \n",(char*)temp);
				  
				busy_block_t block = (busy_block_t) temp;
				temp_size= first_free->size;
				block->size =size;
				  
	     		free_block_t node = (free_block_t) ((char*) block + size);
				node->size = temp_size - block->size; 
				node->next = temp->next;
				if (i==0)
				{
				  first_free = node;
				  
				}
				else 
				{
					temp_before=node; 
				}
				  printf("Allocation Succeeded address of busy %p address of empty %p \n",(char*)block,(char*)node);
					return ((char*)block);
		}
			
			
		else 
		{
			temp_before=temp;
			temp=temp->next;
			i++;
		}
	} while(temp->next != NULL || temp_before->next != NULL);
	printf("No Memory Available");
	return NULL;
 
// print_alloc_info(addr, actual_size);
 }



void memory_free(char *p)
{
//	print_free_info(p); 
	
	if (validAdress(p) == 0)
	{
		printf("Invalid adress !\n");
	}
	else if ( first_free < (free_block_t)p)   // This means that there is a free block before the busy block we are trying to free 
	{
		free_block_t previous_free = getPreviousFreeBlock((busy_block_t)p);
		free_block_t next_free= previous_free->next;
		busy_block_t block = (busy_block_t) p;
		
		if ( previous_free + previous_free->size == (free_block_t)p) // The previous free block is Contiguous
		{
			if( next_free != NULL)                                   // There exist a next free block
			{
				if ( block ==  (busy_block_t)next_free - (block->size))  // Previous block and the next one are Contiguous A
				{
					
					previous_free->size += block->size+next_free->size;
					previous_free->next=next_free->next;
					free(block);
					free(next_free);
					
				} 
				else    												 // previous is Contiguous but the next one is not  D
				{
					previous_free->size += block->size;
					free(block);
				}	
			}		
			else if(next_free == NULL)  						// Previous is contiguous but there is no next free block 
			{
				previous_free->size += block->size;
				free(block);
			}
		}
		else                                         // The previous block is not contigious
		{
			if( next_free != NULL)  // There is a next free block
			{				
				if ( block ==  (busy_block_t)next_free - (block->size))  // next free block is Contigious
				{
					free_block_t tmp = (free_block_t)malloc(sizeof(free_block_s));
					tmp->size = next_free->size + block->size;
					tmp->next = next_free->next;
					// tmp->before = previous_free;   We don't need this line 
					free(next_free);
					previous_free->next = tmp;
					free(block);					
				} 
				else    												 // previous is not contiguous and the next one is not 
				{
					free_block_t tmp = (free_block_t)malloc(sizeof(free_block_s));
					tmp->size = block->size;
					tmp->next = previous_free->next;
					// tmp->before = previous_free;    we don't need this line also 
					previous_free->next = tmp;
					free(block);					
				}	
			}
			
			else if(next_free == NULL)  						// There is no next free block 
			{
				free_block_t tmp = (free_block_t)malloc(sizeof(free_block_s));
				tmp->size = block->size;
				tmp->next = NULL;
				// tmp->before = previous_free;  we don't need this line
				previous_free->next = tmp;
				free(block);
			}
			
		}
	}
	else                                                             //  There is not previous free block 
	{
		busy_block_t block = (busy_block_t) p;
		if ( block ==  (busy_block_t)first_free - (block->size))  // If the first free block after our block is Contigious
		{
			
			if ( block == (busy_block_t) memory)     // and the busy block is the first block in the memory
			{
					free_block_t temp = (free_block_t)malloc(sizeof(free_block_s));
					temp->size=block->size+first_free->size;
					temp->next=first_free;
					free(first_free);  // Because we want to have on Contigious free block
					first_free=temp;
					free(block);								
				
			}
			else
			{	
				free_block_t temp = (free_block_t)malloc(sizeof(free_block_s));
				temp->size=block->size+first_free->size;	
				temp->next=first_free;
				free(first_free);  // Because we want to have on Contigious free block
				first_free=temp;
				free(block);
			}
		}
		else
		{
			if (block == (busy_block_t)memory)   // The block is the first block of the memory and there is no Contigious free block
			{
				free_block_t temp = (free_block_t)malloc(sizeof(free_block_s));
				temp->size= block->size;
				temp->next=first_free;
				first_free=temp;
				free(block);
			}
			
			else 
			{
				free_block_t temp = (free_block_t)malloc(sizeof(free_block_s));
				temp->size=block->size;
				temp->next=first_free;
				free(block);
				first_free=temp;
				
			}
			
			
		}
		
	}

  /* ... */
}

int validAdress(char *p) // return 0 for an invalid address and 1 for a valid 
{
	// Can't use this method as we don't have a pointer in the busy block.
	//~ busy_block_t current_busy = first_occupied;
	//~ 
	//~ if( (busy_block_t) p < first_occupied)
	//~ {
		//~ return 0;
	//~ }
	//~ 
	//~ while( current_busy < (busy_block_t) memory + MEMORY_SIZE)
	//~ {
		//~ if( current_busy == (busy_block_t) p)
		//~ {
			//~ return 1;
		//~ }
		//~ else
		//~ {
			//~ current_busy = current_busy->next;
		//~ }
	//~ }
	//~ return 1;
	
	
	
	free_block_t temp = first_free;
	if ( (free_block_t)p == first_free)
	{
		return 0;
	}
	while( temp < (free_block_t) memory + MEMORY_SIZE)
	{
		if ( ((free_block_t)p >= temp ) && ( (free_block_t)p < (temp+temp->size) )   )
		{
			return 0 ; // Address is in the empty block
		}
		else 
		{
				temp=temp->next;
		}
	}
	return 1;

}

free_block_t getPreviousFreeBlock(busy_block_t block)
{
	// Correct method
	free_block_t result = first_free;
	
	if( (free_block_t) block < first_free)
	{
		return NULL;
	} 
	
	while(( result < (free_block_t) memory + MEMORY_SIZE) && ( result < (free_block_t) block ))
	{
		
			result = result->next;
		
	}
	
	return result;	
}

int contiguous(free_block_t current, busy_block_t block)
{
	if( (current + current->size) == (free_block_t) block )
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

void print_info(void) 
{
  fprintf(stderr, "Memory : [%lu %lu] (%lu bytes)\n", (long unsigned int) 0, (long unsigned int) (memory+MEMORY_SIZE), (long unsigned int) (MEMORY_SIZE));
  fprintf(stderr, "Free block : %lu bytes; busy block : %lu bytes.\n", ULONG(sizeof(free_block_s)), ULONG(sizeof(busy_block_s))); 
}

void print_free_info(char *addr)
{
  if(addr)
    fprintf(stderr, "FREE  at : %lu \n", ULONG(addr - memory)); 
  else
    fprintf(stderr, "FREE  at : %lu \n", ULONG(0)); 
}

void print_alloc_info(char *addr, int size)
{
  if(addr)
  {
    fprintf(stderr, "ALLOC at : %lu (%d byte(s))\n", 
	    ULONG(addr - memory), size);
  }
  else
  {
    fprintf(stderr, "Warning, system is out of memory\n"); 
  }
}

void print_free_blocks(void) 
{
  free_block_t current; 
  fprintf(stderr, "Begin of free block list :\n"); 
  for(current = first_free; current != NULL; current = current->next)
    fprintf(stderr, "Free block at address %lu, size %u\n", ULONG((char*)current - memory), current->size);
}

char *heap_base(void) 
{
  return memory;
}


void *malloc(size_t size)
{
  static int init_flag = 0; 
  if(!init_flag)
  {
    init_flag = 1; 
    memory_init(); 
    //print_info(); 
  }      
  return (void*)memory_alloc((size_t)size); 
}
//~ 
void free(void *p)
{
  if (p == NULL) return;
  memory_free((char*)p); 
  print_free_blocks();
}
//~ 
void *realloc(void *ptr, size_t size)
{
  if(ptr == NULL)
    return memory_alloc(size); 
  busy_block_t bb = ((busy_block_t)ptr) - 1; 
  printf("Reallocating %d bytes to %d\n", bb->size - (int)sizeof(busy_block_s), (int)size); 
  if(size <= bb->size - sizeof(busy_block_s))
    return ptr; 

  char *new = memory_alloc(size); 
  memcpy(new, (void*)(bb+1), bb->size - sizeof(busy_block_s) ); 
  memory_free((char*)(bb+1)); 
  return (void*)(new); 
}
//~ 

#ifdef MAIN
int main(int argc, char **argv)
{

  //~ /* The main can be changed, it is *not* involved in tests */
  memory_init();
  print_info(); 
  printf(" First print \n");
  print_free_blocks();
   printf(" Second  print \n");
  int i ; 
  for( i = 0; i < 10; i++)
  {
	  printf(" Allocate %d  \n",i);
    char *b = memory_alloc(rand()%8);
    printf(" Allocated Address %p  \n",b);
    
    //~ memory_free(b); 
       //~ printf(" Memory is free  %d \n",i);

    print_free_blocks();
  }

	

//~ 
  //~ char * a = memory_alloc(15);
  //~ a=realloc(a, 20); 
  //~ memory_free(a);
//~ 
//~ 
  //~ a = memory_alloc(10);
  //~ memory_free(a);
//~ 
//~ 
	//~ printf("before last \n"); 
  //~ printf("%lu\n",(long unsigned int) (memory_alloc(9)));
  //~ printf("%lu \n %lu " , sizeof(free_block_s),sizeof(busy_block_s));
  //~ 
  //~ return 1;
  return EXIT_SUCCESS;
}
#endif 
