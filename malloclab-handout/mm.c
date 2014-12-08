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
#define INIT_HEAP_SIZE 4048

/**
 * The header structure. We need how big the spot is
 * and if it's currently in use
 **/
typedef struct {
  int size;// = 0;
  char inUse;// = false;
} head_t;


//FUNCTION PROTOTYPES
head_t *getHead(void *allocation);
int getSize(head_t *head);
void *getNext(void *allocation);
void *getFirst();
void *insert(void *addr, int size);
void *findFit(int size);
int getOffset(void *adr);
char getInUse(head_t *head);
void format(void *addr, int size);
void *getLast();
void *update(void *addr, int size);
void *nextByte(void *addr);

void *heap;
int heapSize;

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
    printf("Initializing...\n");
   
    heap = mem_sbrk(INIT_HEAP_SIZE);
    heapSize = INIT_HEAP_SIZE;

    format(heap, INIT_HEAP_SIZE - sizeof(head_t));

    void *first = getFirst();
    printf("Size: %d\n", getSize(getHead(first)));

    mm_malloc(10);
    printf("address of last: %p\n", getLast()); 

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    printf("Call to Malloc with size: %d\n", size);
    
    if (size == 0)
    {
        return NULL;
    }
    int newSize = ALIGN(size + SIZE_T_SIZE);
    
    if (heap == NULL)
    {
        return NULL;
    } 

    void *addr  = findFit(newSize);
    
    while (addr == NULL) 
    {
        printf("Increasing heap size...!\n");
        //not enough space! Increase heap by double

	//update the end allocation to reflect our new size
	int increase = heapSize; //we're going to double, so our increase is
				 //the current heap size
        void *last = getLast(); //returns the last allocation in the list
	mem_sbrk(heapSize); //actually increase the heap size
	heap = mem_heap_lo();
	heapSize *= 2;
        if (getInUse(getHead(last)) == 0) 
        {
	    //not in use, so expand the size
            int csize = getSize(getHead(last));
            csize += increase;
        
	    //update ending thing with it's new size
            update(last, csize);
            (getHead(last))->inUse = 0; //set it to not be in use    

	} else {
	    //in use, which means we just add a new empty allocation at the end
	    void *end = getNext(last);
	    end = ((char *) end - sizeof(head_t)); //adjust for head
	    format(end, increase);
	}
    
       //after increasing heapsize, we need to modify the last element in the list to make sure it accoutns for all the size we have. 

       addr = findFit(newSize);
    }
    
    //we got an address, theoritically
    printf("Found an area to put it!\n"); 
    //get the old size, as we will use it later
    int oldSize = getSize(getHead(addr));   
    void *next = getNext(addr);
    //insert the new record
    addr = update(addr, newSize);

    //addr now points to the byte right after our allocated space
    //if we used all of the space, we're set
    if (newSize < oldSize)
    {
        printf("appending new zone. Sizes (new, old): [%d, %d]i\n", newSize, oldSize);

	//check if there's a memory spot after that's also free
	//if so, merge them. else, make a new spot after
	//void *next = getNext(addr);
	
	if (getOffset(next) <= heapSize)
	{ 
	    //there's something after that's not off our heap
	    int difference = oldSize - newSize;

	    //is it in use?
	    if (getInUse(getHead(next)) == 0) 
	    {
		int thatSize = getSize(getHead(next));
		format(nextByte(addr), difference + thatSize);
	    } else
	    {
                //create a new header detailing 
		format(nextByte(addr), difference);
	    }

	}
        //TODO make it so if there's a nother memory location after
        //     that is not in use, you combine them and their space
    }
    //addr = update(addr, newSize); 
    return addr;
    
    /*
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
    */
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
    if (ptr == NULL)
         return;

    head_t *head = getHead(ptr);
    
    head->inUse = 0;

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
  void *newAlloc = ((char *) allocation + size + sizeof(head_t));

  return (void *) newAlloc; 
}

/**
 * Returns the first allocation in the passed heap.<br />
 * expects the very first address in the heap. That is, this expects the address of the first byte of a header.
 **/
void *getFirst()
{
  int headSize = sizeof(head_t);
  return ((char *) heap + headSize); //move up the first address (a header) by the size of the header
}

/**
 *Returns the allocation that is last in the list.
 **/
void *getLast() 
{
  void *ptr = getFirst();
  int offset;
  while (1)
  {
    //check if the next allocation is outside our range
    offset = getOffset(ptr) + getSize(getHead(ptr) + sizeof(head_t));
    if (offset > heapSize) 
    {
      return ptr;
    }
    //else it wont be outside our range, so keep going
    ptr = getNext(ptr);

    //cycle again
  }

  //return ptr;
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
  
  head->size = size - sizeof(head_t);
  head->inUse = 1;
  
  //increment the passed address by (sizeof header) bytes and return it
  return ((char *) addr +  sizeof(head_t)); 
}

/**
 *Updates the record to the allocation pased
 *This is different from insert in that it doesn't add a new header. Instead it updates it.
 *Returns a pointer to the data section you are allowed to save in
 **/
void *update(void *allocation, int size)
{
    if (allocation == NULL || size <= 0) 
         return NULL;
    
    head_t *head = getHead(allocation);
    head->size = size;
    head->inUse = 1;

    return allocation;
}

/**
 * Marks the address passed as the start of a memory area that is NOT in use
 **/
void format(void *addr, int size) 
{
  if (addr == NULL) 
     return;
  if (size <= 0)
     return;
   
  int offset = getOffset(addr); 
  //make sure the address we were passed is in the heap, and that the size isn't too large
  if (offset < 0 || (offset + size) > heapSize) 
  {
    printf("Error in formatting: too large!\nOffset: %d  size: %d  heapSize: %d\n", offset, size, heapSize);
    //not enough space or outside of bounds
    return;
  }
  
  void *ptr = insert(addr, size);
  head_t *head = getHead(ptr);

  head->inUse = 0;

}

void *findFit(int size)
{
  //get the first allocation address in the list
  void *zone = getFirst();
  
  if (getSize(getHead(zone)) >= size)
  if (getInUse(getHead(zone)) == 0)
  {
      printf("Got it immediately!\n");
      return zone;
  }  

  while (getOffset(zone) < heapSize && getOffset(zone) > 0)
  {
    //it's in our heap. Check it's size and inUse variable

    //get head of the current zone we're looking at
    head_t *head = getHead(zone);
    
    if (getSize(head) >= size)
    if (getInUse(head) == 0)
    {
      //big enough and not in use! we can use it!
      return zone;
    }
    
    //printf("Area too small {size: %d need: %d}\n", getSize(head), size); 
    //we didn't find one that was both avaialble and big enough.
    zone = getNext(zone);
  }
  
  //we never found a zone that fit our requirements
  return NULL;
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

/**
 * Returns the byte-wide indication of if the header indicates that the zone is in use.<br />
 * IN USE: 1    NOT IN USE: 0
 **/
char getInUse(head_t *head) 
{
  if (head == NULL)
  {
    return -1;
  }
  
  return head->inUse;
}

/**
 *Returns the first byte of the area outside the passed allocation
 *@param void *addr the ALLOCATION. This is the first byte after the describing header
 **/
void *nextByte(void *addr)
{
  if (addr == NULL) 
      return NULL;

  head_t *head;
  head = getHead(addr);
  
  return ((char *) addr + getSize(head));
}













