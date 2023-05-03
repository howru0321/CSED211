/*220210054 HaWoo Jung*/

/*
 * malloc allocator based on first-fit mechanism.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*Basic macros*/
#define WSIZE       4       /*size for word and header/footer       | bytes*/
#define DSIZE       8       /*size for double word                  | bytes*/
#define CHUNKSIZE   (1<<12) /*size by which heap would be extended  | bytes*/

#define MAX(x,y)    ((x) > (y)? (x) : (y))

/* pack a size information and allocation bit into a word for header */
#define PACK(size, alloc) ((size)|(alloc))

/*to handle a word from address p */
#define GET(p)          (*(unsigned int*)(p))               /*read a value pointed by pointer p*/
#define PUT(p, val)     (*(unsigned int*)(p) = (val))       /*assign a value to pointer p*/

/*get the size and information about allocating*/
#define GET_SIZE(p)     (GET(p) & ~0x7)
#define GET_ALLOC(p)    (GET(p) & 0x1)

/*for a given bp, a block pointer, get addresses of block's header or footer*/
#define HDRP(bp)    ((typeof(bp))(bp) - WSIZE)
#define FTRP(bp)    ((typeof(bp))(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/*for a given bp, get address of blocks' next or previous one*/
#define NEXT_BLKP(bp)   ((typeof(bp))(bp) + GET_SIZE((typeof(bp))(bp) - WSIZE))
#define PREV_BLKP(bp)   ((typeof(bp))(bp) - GET_SIZE((typeof(bp))(bp) - DSIZE))

#define PREV_FBLKP(bp) (*(void **) (bp))         
#define NEXT_FBLKP(bp) (*(void **) (bp + WSIZE))   
/*constants*/
/*pointer that points the first */
static char *heap_listp;

#define EVEN(size, words)    (size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE)


static void *find_fit(size_t size){
    /*best-fit search*/
    void *bp;
    void *temp=NULL;

    for(bp = heap_listp; GET_SIZE(HDRP(bp))>0; bp = NEXT_BLKP(bp) ){ 
        if((size<=GET_SIZE(HDRP(bp)))&&!GET_ALLOC(HDRP(bp))){
            if(temp==NULL)
                temp=bp;
            else{
                if(GET_SIZE(HDRP(bp))<GET_SIZE(HDRP(temp)))
                    temp=bp;
            }
        }
    }
    return temp;
}

static void* coalesce(void* bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    
    if (prev_alloc && next_alloc) { /* Case 1 */
        return bp;
    }
    else if (prev_alloc && !next_alloc) { /* Case 2 */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc) { /* Case 3 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    } 
    else { /* Case 4 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
            GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

static void* extend_heap(size_t words)
{
    char* bp;
    size_t size;
    
    /* Allocate an even number of words to maintain alignment */
    EVEN(size,words);
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    
    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0)); /* Free block header */
    PUT(FTRP(bp), PACK(size, 0)); /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */
    
    /* Coalesce if the previous block was free */
    return bp;
}


static void place(void* bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    
    if ((csize - asize) >= (2 * DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    }
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void*)-1)
        return -1;
    PUT(heap_listp, 0); /* Alignment padding */
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1)); /* Epilogue header */
    heap_listp += (2 * WSIZE);
   
        /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(4) == NULL)
        return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by finding proper free block
 */
void *mm_malloc(size_t size)
{
    if(size == 0)
        return NULL;
    
    int newsize = ALIGN(size + SIZE_T_SIZE);
    int new_word = newsize/WSIZE;
    EVEN(newsize,new_word);
    char *bp = find_fit(newsize);
    
    if(bp != NULL){
        place(bp, newsize);
        return bp;
    }
    else{
        if((bp = extend_heap(newsize/WSIZE)) != NULL){
            place(bp, newsize);
            return bp;
        }
        else
            return NULL;
    } 
}

/*
 * mm_free - Free a block
 */
void mm_free(void *bp)
{
    if(bp == NULL)
        return;
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * mm_realloc - Implemented by using mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    if (size == 0) {                
        mm_free(oldptr);
        return NULL;
    }

    if (oldptr == NULL)              
        return mm_malloc(size);
        
    size_t newsize = ALIGN(size + SIZE_T_SIZE);
    int newwords = newsize/WSIZE;
    EVEN(newsize,newwords);
    size_t curr_size = GET_SIZE(HDRP(oldptr));

    if(newsize<=curr_size)
        return oldptr;

    if(!GET_ALLOC(HDRP(NEXT_BLKP(oldptr)))){
        size_t extend_size = curr_size+GET_SIZE(HDRP(NEXT_BLKP(oldptr)));
        if(extend_size >= newsize){
            PUT(HDRP(oldptr), PACK(extend_size,1));
            PUT(FTRP(oldptr), PACK(extend_size,1));
            return oldptr;
        }
    }
    else{
        void* newptr = mm_malloc(newsize);
        memcpy(newptr, oldptr, newsize);
        mm_free(oldptr);
        return newptr;
    }
}
