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
    "ateam",
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
 * A record of the start of a <b>free area</b>  on our heap.<br />
 * Will keep track of the starting address of the free space as well as how big it is
 *
 **/ 
typedef struct t_heap_entry  {
    //how much space is allocated
    int size;
    //the actual address
    void *address = NULL;
    
    
} entry;

typedef struct t_list_node {
    //next node in list
    t_list_node *next = NULL;
    //prev node in list
    t_list_node *prev = NULL;
    //address linked
    void *ptr addr = NULL;
    
} node_t;

typedef struct t_linked_list {
    
    t_list_node *head = NULL;
    t_list_node *tail = NULL;

    int size = 0;
} llist;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
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


llist *newList(void) {
    llist *list;
    list = malloc(size_t * sizeof(llist));
    
    node_t
    listAdd(list,
    
    
    
}

node_t *newNode(void *addr) {
    node_t *node;
    node = malloc(size_t * sizeof(t_node));
    
    return node;
     
}

void listAdd(llist list, node_t *node) {
    if (node == NULL) {
       return;
    }
   
    list->tail->next = node;
    list->tail = node;
    
    list->size++; 
} 









