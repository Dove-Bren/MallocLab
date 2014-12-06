/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Team Awesome & Cool",
    /* First member's full name */
    "Skyler Manzanares",
    /* First member's email address */
    "smanzana@nmt.edu",
    /* Second member's full name */
    "James Klarkowski",
    /* Second member's email address */
    "trassix@nmt.edu"
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))


/**
 * The initial size of our heap
 **/
#define HEAP_SIZE 4048

/**
 * The header structure. We need how big the spot is
 * and if it's currently in use
 **/
typedef struct {
  int size;// = 0;
  char inUse;// = false;
} head_t;

void *heap;

/* 
 * mm_init - initialize the malloc package.
 *
 * The mm.c from CS:APP uses the following to initalize:
 *
 * Allign
 * Prolog[head(size8, allocated = yes), foot(size = 8, alloc = yes)]
 * Epilog[head(size = 0, allocated = yes)]
 * place heap list pointer
 * extend heap
 * 
 * alternativly, would we want to initially set a heap size by defining the epilog 
 *n blocks from the footer? then we can skip extending the heap during the initalization.
 * I think yes. That sounds better
 */
int mm_init(void)
{
    heap = mem_sbrk(HEAP_SIZE);
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 *
 * All we really need to do here is set the allocated bit to 0 for the header
 *and footer, and put it address in the list (is that right?).
 *I don't think we dont' need to zero out the payload.
 *
 * If we want to, we can coalesce here too (CS:APP does), but it is not needed.
 *
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

/**
 * Returns the header of the allocated block.<br />
 * This assumes that the passed address is the first byte in the allocation.<br />
 * If it is not, junk can be expected to be returned.
 * @param void *allocation a pointer to the first address in the allocated space
 * @return a head structure describing the allocated area
 **/
head_t *getHead(void *allocation) 
{
  //get a pointer to (allocation - n bytes) where n is the number of bytes that make up the header
  void *headPtr = (char *) allocation - (sizeof(char) * sizeof(head_t));

  //cast this to a head
  head_t *head = ((head_t *) headPtr);

  return head;
}

/**
 * Return the size contianed in the passed header
 **/
int getSize(head_t *head) 
{
  if (head == NULL) 
  {
     return 0;
  }
  
  return (head->size);
}

/**
 * Returns the next allocation in the heap after the passed one
 **/
void *getNext(void *allocation)
{
  head_t *head = getHead(allocation);
  int size = getSize(head);

  //move our pointer up (size) bytes
  void *newAlloc = ((char *) allocation + size);

  return (void *) newAlloc; 
}

/**
 * Returns the first allocation in the passed heap.<br />
 * expects the very first address in the heap. That is, this expects the address of the first byte of a header.
 **/
void *getFirst()
{
  int headSize = sizeof(char) * sizeof(head_t);
  return ((char *) heap + headSize); //move up the first address (a header) by the size of the header
}

/**
 * Sets the privded area as used.<br />
 * This is done by first inserting a header, and then returning the address
 * of the space left for use<br />
 * @param void* addr the address that the first byte of the header will be at
 * @param int size the size OF THE MEMORY ALLOCATION excluding the header
 * @return the address of the allocation area.
 **/
void *insert(void *addr, int size)
{
  //give us an easy way to access the different pieces of the header by casting it to a header :DDD
  head_t *head = ((head_t *) addr);
  
  head->size = size;
  head->inUse = 1;
  
  //increment the passed address by (sizeof header) bytes and return it
  return ((char *) addr + (sizeof(char) * sizeof(head_t))); 
}

void *findFit(int size)
{
  //get the first allocation address in the list
  void *zone = getFirst();
  
  while 
} 

/**
 *Returns how many bytes the passed address is from the start of the heap.<br />
 *This function makes no attept to check if the passed address is outside of our heap.
 **/
int getOffset(void *addr) 
{
  if (addr == NULL) 
  {
    return 0;
  }
  
  //get the difference in addrseses
  int offset = ((char *) addr - (char *) heap);
  //We don't care if it's negative or outisde of our range. This just returns how off the passed address is
  return offset; 
}

















