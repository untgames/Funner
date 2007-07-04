// Copyright (C) 2004 Id Software, Inc.
//

#pragma hdrstop

#include <assert.h>
#include <malloc.h>
#include <memory.h>

typedef struct {
  int   num;
  int   minSize;
  int   maxSize;
  int   totalSize;
} memoryStats_t;

//===============================================================
//
//  idHeap
//
//===============================================================

#define SMALL_HEADER_SIZE   ( (int) ( sizeof( unsigned char ) + sizeof( unsigned char ) ) )
#define MEDIUM_HEADER_SIZE    ( (int) ( sizeof( mediumHeapEntry_s ) + sizeof( unsigned char ) ) )
#define LARGE_HEADER_SIZE   ( (int) ( sizeof( size_t * ) + sizeof( unsigned char ) ) )

#define ALIGN_SIZE( bytes )   ( ( (bytes) + ALIGN - 1 ) & ~(ALIGN - 1) )
#define SMALL_ALIGN( bytes )  ( ALIGN_SIZE( (bytes) + SMALL_HEADER_SIZE ) - SMALL_HEADER_SIZE )
#define MEDIUM_SMALLEST_SIZE  ( ALIGN_SIZE( 256 ) + ALIGN_SIZE( MEDIUM_HEADER_SIZE ) )


class idHeap {

public:
          idHeap( void );
          ~idHeap( void );        // frees all associated data
  void      Init( void );         // initialize
  void *      Allocate( const size_t bytes );  // allocate memory
  void      Free( void *p );        // free memory
  void *      Allocate16( const size_t bytes );// allocate 16 unsigned char aligned memory
  void      Free16( void *p );        // free 16 unsigned char aligned memory
  size_t     Msize( void *p );       // return size of data block
  void      Dump( void  );

  void      AllocDefragBlock( void );   // hack for huge renderbumps

private:

  enum {
    ALIGN = 8                 // memory alignment in bytes
  };

  enum {
    INVALID_ALLOC = 0xdd,
    SMALL_ALLOC   = 0xaa,           // small allocation
    MEDIUM_ALLOC  = 0xbb,           // medium allocaction
    LARGE_ALLOC   = 0xcc            // large allocaction
  };

  struct page_s {                 // allocation page
    void *        data;         // data pointer to allocated memory
    size_t       dataSize;       // number of bytes of memory 'data' points to
    page_s *      next;         // next free page in same page manager
    page_s *      prev;         // used only when allocated
    size_t       largestFree;      // this data used by the medium-size heap manager
    void *        firstFree;        // pointer to first free entry
  };

  struct mediumHeapEntry_s {
    page_s *      page;         // pointer to page
    size_t       size;         // size of block
    mediumHeapEntry_s * prev;         // previous block
    mediumHeapEntry_s * next;         // next block
    mediumHeapEntry_s * prevFree;       // previous free block
    mediumHeapEntry_s * nextFree;       // next free block
    size_t       freeBlock;        // non-zero if free block
  };

  // variables
  void *      smallFirstFree[256/ALIGN+1];  // small heap allocator lists (for allocs of 1-255 bytes)
  page_s *    smallCurPage;         // current page for small allocations
  size_t     smallCurPageOffset;       // unsigned char offset in current page
  page_s *    smallFirstUsedPage;       // first used page of the small heap manager

  page_s *    mediumFirstFreePage;      // first partially free page
  page_s *    mediumLastFreePage;       // last partially free page
  page_s *    mediumFirstUsedPage;      // completely used page

  page_s *    largeFirstUsedPage;       // first page used by the large heap manager

  page_s *    swapPage;

  size_t     pagesAllocated;         // number of pages currently allocated
  size_t     pageSize;           // size of one alloc page in bytes

  size_t     pageRequests;         // page requests
  size_t     OSAllocs;           // number of allocs made to the OS

  int       c_heapAllocRunningCount;

  void      *defragBlock;         // a single huge block that can be allocated
                          // at startup, then freed when needed

  // methods
  page_s *    AllocatePage( size_t bytes );  // allocate page from the OS
  void      FreePage( idHeap::page_s *p );  // free an OS allocated page

  void *      SmallAllocate( size_t bytes ); // allocate memory (1-255 bytes) from small heap manager
  void      SmallFree( void *ptr );     // free memory allocated by small heap manager

  void *      MediumAllocateFromPage( idHeap::page_s *p, size_t sizeNeeded );
  void *      MediumAllocate( size_t bytes );  // allocate memory (256-32768 bytes) from medium heap manager
  void      MediumFree( void *ptr );    // free memory allocated by medium heap manager

  void *      LargeAllocate( size_t bytes ); // allocate large block from OS directly
  void      LargeFree( void *ptr );     // free memory allocated by large heap manager

  void      ReleaseSwappedPages( void );
  void      FreePageReal( idHeap::page_s *p );
};


/*
================
idHeap::Init
================
*/
void idHeap::Init () {
  OSAllocs      = 0;
  pageRequests    = 0;
  pageSize      = 65536 - sizeof( idHeap::page_s );
  pagesAllocated    = 0;                // reset page allocation counter

  largeFirstUsedPage  = NULL;               // init large heap manager
  swapPage      = NULL;

  memset( smallFirstFree, 0, sizeof(smallFirstFree) );  // init small heap manager
  smallFirstUsedPage  = NULL;
  smallCurPage    = AllocatePage( pageSize );
  assert( smallCurPage );
  smallCurPageOffset  = SMALL_ALIGN( 0 );

  defragBlock = NULL;

  mediumFirstFreePage = NULL;               // init medium heap manager
  mediumLastFreePage  = NULL;
  mediumFirstUsedPage = NULL;

  c_heapAllocRunningCount = 0;
}

/*
================
idHeap::idHeap
================
*/
idHeap::idHeap( void ) {
  Init();
}

/*
================
idHeap::~idHeap

  returns all allocated memory back to OS
================
*/
idHeap::~idHeap( void ) {

  idHeap::page_s  *p;

  if ( smallCurPage ) {
    FreePage( smallCurPage );     // free small-heap current allocation page
  }
  p = smallFirstUsedPage;         // free small-heap allocated pages 
  while( p ) {
    idHeap::page_s *next = p->next;
    FreePage( p );
    p= next;
  }

  p = largeFirstUsedPage;         // free large-heap allocated pages
  while( p ) {
    idHeap::page_s *next = p->next;
    FreePage( p );
    p = next;
  }

  p = mediumFirstFreePage;        // free medium-heap allocated pages
  while( p ) {
    idHeap::page_s *next = p->next;
    FreePage( p );
    p = next;
  }

  p = mediumFirstUsedPage;        // free medium-heap allocated completely used pages
  while( p ) {
    idHeap::page_s *next = p->next;
    FreePage( p );
    p = next;
  }

  ReleaseSwappedPages();      

  if ( defragBlock ) {
    free( defragBlock );
  }

  assert( pagesAllocated == 0 );
}

/*
================
idHeap::AllocDefragBlock
================
*/
void idHeap::AllocDefragBlock( void ) {
  int   size = 0x40000000;

  if ( defragBlock ) {
    return;
  }
  while( 1 ) {
    defragBlock = malloc( size );
    if ( defragBlock ) {
      break;
    }
    size >>= 1;
  }
//  idLib::common->Printf( "Allocated a %i mb defrag block\n", size / (1024*1024) );
}

/*
================
idHeap::Allocate
================
*/
void *idHeap::Allocate( const size_t bytes ) {
  if ( !bytes ) {
    return NULL;
  }
  c_heapAllocRunningCount++;

#if USE_LIBC_MALLOC
  return malloc( bytes );
#else
  if ( !(bytes & ~255) ) {
    return SmallAllocate( bytes );
  }
  if ( !(bytes & ~32767) ) {
    return MediumAllocate( bytes );
  }
  return LargeAllocate( bytes );
#endif
}

/*
================
idHeap::Free
================
*/
void idHeap::Free( void *p ) {
  if ( !p ) {
    return;
  }
  c_heapAllocRunningCount--;

#if USE_LIBC_MALLOC
  free( p );
#else
  switch( ((unsigned char *)(p))[-1] ) {
    case SMALL_ALLOC: {
      SmallFree( p );
      break;
    }
    case MEDIUM_ALLOC: {
      MediumFree( p );
      break;
    }
    case LARGE_ALLOC: {
      LargeFree( p );
      break;
    }
    default: {
//      idLib::common->FatalError( "idHeap::Free: invalid memory block (%s)", idLib::sys->GetCallStackCurStr( 4 ) );
      break;
    }
  }
#endif
}

/*
================
idHeap::Allocate16
================
*/
void *idHeap::Allocate16( const size_t bytes ) {
  unsigned char *ptr, *alignedPtr;

  ptr = (unsigned char *) malloc( bytes + 16 + 4 );
  if ( !ptr ) {
    if ( defragBlock ) {
//      idLib::common->Printf( "Freeing defragBlock on alloc of %i.\n", bytes );
      free( defragBlock );
      defragBlock = NULL;
      ptr = (unsigned char *) malloc( bytes + 16 + 4 );      
      AllocDefragBlock();
    }
    if ( !ptr ) {
//      common->FatalError( "malloc failure for %i", bytes );
    }
  }
  alignedPtr = (unsigned char *) ( ( (int) ptr ) + 15 & ~15 );
  if ( alignedPtr - ptr < 4 ) {
    alignedPtr += 16;
  }
  *((int *)(alignedPtr - 4)) = (int) ptr;
  return (void *) alignedPtr;
}

/*
================
idHeap::Free16
================
*/
void idHeap::Free16( void *p ) {
  free( (void *) *((int *) (( (unsigned char *) p ) - 4)) );
}

/*
================
idHeap::Msize

  returns size of allocated memory block
  p = pointer to memory block
  Notes:  size may not be the same as the size in the original
      allocation request (due to block alignment reasons).
================
*/
size_t idHeap::Msize( void *p ) {

  if ( !p ) {
    return 0;
  }

#if USE_LIBC_MALLOC
  #ifdef _WIN32
    return _msize( p );
  #else
    return 0;
  #endif
#else
  switch( ((unsigned char *)(p))[-1] ) {
    case SMALL_ALLOC: {
      return SMALL_ALIGN( ((unsigned char *)(p))[-SMALL_HEADER_SIZE] * ALIGN );
    }
    case MEDIUM_ALLOC: {
      return ((mediumHeapEntry_s *)(((unsigned char *)(p)) - ALIGN_SIZE( MEDIUM_HEADER_SIZE )))->size - ALIGN_SIZE( MEDIUM_HEADER_SIZE );
    }
    case LARGE_ALLOC: {
      return ((idHeap::page_s*)(*((size_t *)(((unsigned char *)p) - ALIGN_SIZE( LARGE_HEADER_SIZE )))))->dataSize - ALIGN_SIZE( LARGE_HEADER_SIZE );
    }
    default: {
//      idLib::common->FatalError( "idHeap::Msize: invalid memory block (%s)", idLib::sys->GetCallStackCurStr( 4 ) );
      return 0;
    }
  }
#endif
}

/*
================
idHeap::FreePageReal

  frees page to be used by the OS
  p = page to free
================
*/
void idHeap::FreePageReal( idHeap::page_s *p ) {
  assert( p );
  ::free( p );
}

/*
================
idHeap::ReleaseSwappedPages

  releases the swap page to OS
================
*/
void idHeap::ReleaseSwappedPages () {
  if ( swapPage ) {
    FreePageReal( swapPage );
  }
  swapPage = NULL;
}

/*
================
idHeap::AllocatePage

  allocates memory from the OS
  bytes = page size in bytes
  returns pointer to page
================
*/
idHeap::page_s* idHeap::AllocatePage( size_t bytes ) {
  idHeap::page_s* p;

  pageRequests++;

  if ( swapPage && swapPage->dataSize == bytes ) {      // if we've got a swap page somewhere
    p     = swapPage;
    swapPage  = NULL;
  }
  else {
    size_t size;

    size = bytes + sizeof(idHeap::page_s);

    p = (idHeap::page_s *) ::malloc( size + ALIGN - 1 );
    if ( !p ) {
      if ( defragBlock ) {
//        idLib::common->Printf( "Freeing defragBlock on alloc of %i.\n", size + ALIGN - 1 );
        free( defragBlock );
        defragBlock = NULL;
        p = (idHeap::page_s *) ::malloc( size + ALIGN - 1 );      
        AllocDefragBlock();
      }
      if ( !p ) {
//        common->FatalError( "malloc failure for %i", bytes );
      }
    }

    p->data   = (void *) ALIGN_SIZE( (int)((unsigned char *)(p)) + sizeof( idHeap::page_s ) );
    p->dataSize = size - sizeof(idHeap::page_s);
    p->firstFree = NULL;
    p->largestFree = 0;
    OSAllocs++;
  }

  p->prev = NULL;
  p->next = NULL;

  pagesAllocated++;
  
  return p;
}

/*
================
idHeap::FreePage

  frees a page back to the operating system
  p = pointer to page
================
*/
void idHeap::FreePage( idHeap::page_s *p ) {
  assert( p );

  if ( p->dataSize == pageSize && !swapPage ) {     // add to swap list?
    swapPage = p;
  }
  else {
    FreePageReal( p );
  }

  pagesAllocated--;
}

//===============================================================
//
//  small heap code
//
//===============================================================

/*
================
idHeap::SmallAllocate

  allocate memory (1-255 bytes) from the small heap manager
  bytes = number of bytes to allocate
  returns pointer to allocated memory
================
*/
void *idHeap::SmallAllocate( size_t bytes ) {
  // we need the at least sizeof( size_t ) bytes for the free list
  if ( bytes < sizeof( size_t ) ) {
    bytes = sizeof( size_t );
  }

  // increase the number of bytes if necessary to make sure the next small allocation is aligned
  bytes = SMALL_ALIGN( bytes );

  unsigned char *smallBlock = (unsigned char *)(smallFirstFree[bytes / ALIGN]);
  if ( smallBlock ) {
    size_t *link = (size_t *)(smallBlock + SMALL_HEADER_SIZE);
    smallBlock[1] = SMALL_ALLOC;          // allocation identifier
    smallFirstFree[bytes / ALIGN] = (void *)(*link);
    return (void *)(link);
  }

  size_t bytesLeft = (long)(pageSize) - smallCurPageOffset;
  // if we need to allocate a new page
  if ( bytes >= bytesLeft ) {

    smallCurPage->next  = smallFirstUsedPage;
    smallFirstUsedPage  = smallCurPage;
    smallCurPage    = AllocatePage( pageSize );
    if ( !smallCurPage ) {
      return NULL;
    }
    // make sure the first allocation is aligned
    smallCurPageOffset  = SMALL_ALIGN( 0 );
  }

  smallBlock      = ((unsigned char *)smallCurPage->data) + smallCurPageOffset;
  smallBlock[0]   = (unsigned char)(bytes / ALIGN);    // write # of bytes/ALIGN
  smallBlock[1]   = SMALL_ALLOC;          // allocation identifier
  smallCurPageOffset  += bytes + SMALL_HEADER_SIZE; // increase the offset on the current page
  return ( smallBlock + SMALL_HEADER_SIZE );      // skip the first two bytes
}

/*
================
idHeap::SmallFree

  frees a block of memory allocated by SmallAllocate() call
  data = pointer to block of memory
================
*/
void idHeap::SmallFree( void *ptr ) {
  ((unsigned char *)(ptr))[-1] = INVALID_ALLOC;

  unsigned char *d = ( (unsigned char *)ptr ) - SMALL_HEADER_SIZE;
  size_t *dt = (size_t *)ptr;
  // index into the table with free small memory blocks
  size_t ix = *d;

  // check if the index is correct
  if ( ix > (256 / ALIGN) ) {
//    idLib::common->FatalError( "SmallFree: invalid memory block" );
  }

  *dt = (size_t)smallFirstFree[ix];  // write next index
  smallFirstFree[ix] = (void *)d;   // link
}

//===============================================================
//
//  medium heap code
//
//  Medium-heap allocated pages not returned to OS until heap destructor
//  called (re-used instead on subsequent medium-size malloc requests).
//
//===============================================================

/*
================
idHeap::MediumAllocateFromPage

  performs allocation using the medium heap manager from a given page
  p       = page
  sizeNeeded  = # of bytes needed
  returns pointer to allocated memory
================
*/
void *idHeap::MediumAllocateFromPage( idHeap::page_s *p, size_t sizeNeeded ) {

  mediumHeapEntry_s *best,*nw = NULL;
  unsigned char        *ret;

  best = (mediumHeapEntry_s *)(p->firstFree);     // first block is largest

  assert( best );
  assert( best->size == p->largestFree );
  assert( best->size >= sizeNeeded );

  // if we can allocate another block from this page after allocating sizeNeeded bytes
  if ( best->size >= (size_t)( sizeNeeded + MEDIUM_SMALLEST_SIZE ) ) {
    nw = (mediumHeapEntry_s *)((unsigned char *)best + best->size - sizeNeeded);
    nw->page    = p;
    nw->prev    = best;
    nw->next    = best->next;
    nw->prevFree  = NULL;
    nw->nextFree  = NULL;
    nw->size    = sizeNeeded;
    nw->freeBlock = 0;      // used block
    if ( best->next ) {
      best->next->prev = nw;
    }
    best->next  = nw;
    best->size  -= sizeNeeded;
    
    p->largestFree = best->size;
  }
  else {
    if ( best->prevFree ) {
      best->prevFree->nextFree = best->nextFree;
    }
    else {
      p->firstFree = (void *)best->nextFree;
    }
    if ( best->nextFree ) {
      best->nextFree->prevFree = best->prevFree;
    }

    best->prevFree  = NULL;
    best->nextFree  = NULL;
    best->freeBlock = 0;      // used block
    nw = best;

    p->largestFree = 0;
  }

  ret   = (unsigned char *)(nw) + ALIGN_SIZE( MEDIUM_HEADER_SIZE );
  ret[-1] = MEDIUM_ALLOC;   // allocation identifier

  return (void *)(ret);
}

/*
================
idHeap::MediumAllocate

  allocate memory (256-32768 bytes) from medium heap manager
  bytes = number of bytes to allocate
  returns pointer to allocated memory
================
*/
void *idHeap::MediumAllocate( size_t bytes ) {
  idHeap::page_s    *p;
  void        *data;

  size_t sizeNeeded = ALIGN_SIZE( bytes ) + ALIGN_SIZE( MEDIUM_HEADER_SIZE );

  // find first page with enough space
  for ( p = mediumFirstFreePage; p; p = p->next ) {
    if ( p->largestFree >= sizeNeeded ) {
      break;
    }
  }

  if ( !p ) {               // need to allocate new page?
    p = AllocatePage( pageSize );
    if ( !p ) {
      return NULL;          // malloc failure!
    }
    p->prev   = NULL;
    p->next   = mediumFirstFreePage;
    if (p->next) {
      p->next->prev = p;
    }
    else {
      mediumLastFreePage  = p;
    }

    mediumFirstFreePage   = p;
    
    p->largestFree  = pageSize;
    p->firstFree  = (void *)p->data;

    mediumHeapEntry_s *e;
    e       = (mediumHeapEntry_s *)(p->firstFree);
    e->page     = p;
    // make sure ((unsigned char *)e + e->size) is aligned
    e->size     = pageSize & ~(ALIGN - 1);
    e->prev     = NULL;
    e->next     = NULL;
    e->prevFree   = NULL;
    e->nextFree   = NULL;
    e->freeBlock  = 1;
  }

  data = MediumAllocateFromPage( p, sizeNeeded );   // allocate data from page

    // if the page can no longer serve memory, move it away from free list
  // (so that it won't slow down the later alloc queries)
  // this modification speeds up the pageWalk from O(N) to O(sqrt(N))
  // a call to free may swap this page back to the free list

  if ( p->largestFree < MEDIUM_SMALLEST_SIZE ) {
    if ( p == mediumLastFreePage ) {
      mediumLastFreePage = p->prev;
    }

    if ( p == mediumFirstFreePage ) {
      mediumFirstFreePage = p->next;
    }

    if ( p->prev ) {
      p->prev->next = p->next;
    }
    if ( p->next ) {
      p->next->prev = p->prev;
    }

    // link to "completely used" list
    p->prev = NULL;
    p->next = mediumFirstUsedPage;
    if ( p->next ) {
      p->next->prev = p;
    }
    mediumFirstUsedPage = p;
    return data;
  } 

  // re-order linked list (so that next malloc query starts from current
  // matching block) -- this speeds up both the page walks and block walks

  if ( p != mediumFirstFreePage ) {
    assert( mediumLastFreePage );
    assert( mediumFirstFreePage );
    assert( p->prev);

    mediumLastFreePage->next  = mediumFirstFreePage;
    mediumFirstFreePage->prev = mediumLastFreePage;
    mediumLastFreePage      = p->prev;
    p->prev->next       = NULL;
    p->prev           = NULL;
    mediumFirstFreePage     = p;
  }

  return data;
}

/*
================
idHeap::MediumFree

  frees a block allocated by the medium heap manager
  ptr = pointer to data block
================
*/
void idHeap::MediumFree( void *ptr ) {
  ((unsigned char *)(ptr))[-1] = INVALID_ALLOC;

  mediumHeapEntry_s *e = (mediumHeapEntry_s *)((unsigned char *)ptr - ALIGN_SIZE( MEDIUM_HEADER_SIZE ));
  idHeap::page_s    *p = e->page;
  bool        isInFreeList;

  isInFreeList = p->largestFree >= MEDIUM_SMALLEST_SIZE;

  assert( e->size );
  assert( e->freeBlock == 0 );

  mediumHeapEntry_s *prev = e->prev;

  // if the previous block is free we can merge
  if ( prev && prev->freeBlock ) {
    prev->size += e->size;
    prev->next = e->next;
    if ( e->next ) {
      e->next->prev = prev;
    }
    e = prev;
  }
  else {
    e->prevFree   = NULL;       // link to beginning of free list
    e->nextFree   = (mediumHeapEntry_s *)p->firstFree;
    if ( e->nextFree ) {
      assert( !(e->nextFree->prevFree) );
      e->nextFree->prevFree = e;
    }

    p->firstFree  = e;
    p->largestFree  = e->size;
    e->freeBlock  = 1;        // mark block as free
  }
      
  mediumHeapEntry_s *next = e->next;

  // if the next block is free we can merge
  if ( next && next->freeBlock ) {
    e->size += next->size;
    e->next = next->next;
    
    if ( next->next ) {
      next->next->prev = e;
    }
    
    if ( next->prevFree ) {
      next->prevFree->nextFree = next->nextFree;
    }
    else {
      assert( next == p->firstFree );
      p->firstFree = next->nextFree;
    }

    if ( next->nextFree ) {
      next->nextFree->prevFree = next->prevFree;
    }
  }

  if ( p->firstFree ) {
    p->largestFree = ((mediumHeapEntry_s *)(p->firstFree))->size;
  }
  else {
    p->largestFree = 0;
  }

  // did e become the largest block of the page ?

  if ( e->size > p->largestFree ) {
    assert( e != p->firstFree );
    p->largestFree = e->size;

    if ( e->prevFree ) {
      e->prevFree->nextFree = e->nextFree;
    }
    if ( e->nextFree ) {
      e->nextFree->prevFree = e->prevFree;
    }
    
    e->nextFree = (mediumHeapEntry_s *)p->firstFree;
    e->prevFree = NULL;
    if ( e->nextFree ) {
      e->nextFree->prevFree = e;
    }
    p->firstFree = e;
  }

  // if page wasn't in free list (because it was near-full), move it back there
  if ( !isInFreeList ) {

    // remove from "completely used" list
    if ( p->prev ) {
      p->prev->next = p->next;
    }
    if ( p->next ) {
      p->next->prev = p->prev;
    }
    if ( p == mediumFirstUsedPage ) {
      mediumFirstUsedPage = p->next;
    }

    p->next = NULL;
    p->prev = mediumLastFreePage;

    if ( mediumLastFreePage ) {
      mediumLastFreePage->next = p;
    }
    mediumLastFreePage = p;
    if ( !mediumFirstFreePage ) {
      mediumFirstFreePage = p;
    }
  } 
}

//===============================================================
//
//  large heap code
//
//===============================================================

/*
================
idHeap::LargeAllocate

  allocates a block of memory from the operating system
  bytes = number of bytes to allocate
  returns pointer to allocated memory
================
*/
void *idHeap::LargeAllocate( size_t bytes ) {
  idHeap::page_s *p = AllocatePage( bytes + ALIGN_SIZE( LARGE_HEADER_SIZE ) );

  assert( p );

  if ( !p ) {
    return NULL;
  }

  unsigned char *  d = (unsigned char*)(p->data) + ALIGN_SIZE( LARGE_HEADER_SIZE );
  size_t * dw  = (size_t*)(d - ALIGN_SIZE( LARGE_HEADER_SIZE ));
  dw[0]   = (size_t)p;       // write pointer back to page table
  d[-1]   = LARGE_ALLOC;      // allocation identifier

  // link to 'large used page list'
  p->prev = NULL;
  p->next = largeFirstUsedPage;
  if ( p->next ) {
    p->next->prev = p;
  }
  largeFirstUsedPage = p;

  return (void *)(d);
}

/*
================
idHeap::LargeFree

  frees a block of memory allocated by the 'large memory allocator'
  p = pointer to allocated memory
================
*/
void idHeap::LargeFree( void *ptr) {
  idHeap::page_s* pg;

  ((unsigned char *)(ptr))[-1] = INVALID_ALLOC;

  // get page pointer
  pg = (idHeap::page_s *)(*((size_t *)(((unsigned char *)ptr) - ALIGN_SIZE( LARGE_HEADER_SIZE ))));

  // unlink from doubly linked list
  if ( pg->prev ) {
    pg->prev->next = pg->next;
  }
  if ( pg->next ) {
    pg->next->prev = pg->prev;
  }
  if ( pg == largeFirstUsedPage ) {
    largeFirstUsedPage = pg->next;
  }
  pg->next = pg->prev = NULL;

  FreePage(pg);
}

//===============================================================
//
//  memory allocation all in one place
//
//===============================================================

#undef new

static idHeap *     mem_heap = NULL;
static memoryStats_t  mem_total_allocs = { 0, 0x0fffffff, -1, 0 };
static memoryStats_t  mem_frame_allocs;
static memoryStats_t  mem_frame_frees;

/*
==================
Mem_ClearFrameStats
==================
*/
void Mem_ClearFrameStats( void ) {
  mem_frame_allocs.num = mem_frame_frees.num = 0;
  mem_frame_allocs.minSize = mem_frame_frees.minSize = 0x0fffffff;
  mem_frame_allocs.maxSize = mem_frame_frees.maxSize = -1;
  mem_frame_allocs.totalSize = mem_frame_frees.totalSize = 0;
}

/*
==================
Mem_GetFrameStats
==================
*/
void Mem_GetFrameStats( memoryStats_t &allocs, memoryStats_t &frees ) {
  allocs = mem_frame_allocs;
  frees = mem_frame_frees;
}

/*
==================
Mem_GetStats
==================
*/
void Mem_GetStats( memoryStats_t &stats ) {
  stats = mem_total_allocs;
}

/*
==================
Mem_UpdateStats
==================
*/
void Mem_UpdateStats( memoryStats_t &stats, int size ) {
  stats.num++;
  if ( size < stats.minSize ) {
    stats.minSize = size;
  }
  if ( size > stats.maxSize ) {
    stats.maxSize = size;
  }
  stats.totalSize += size;
}

/*
==================
Mem_UpdateAllocStats
==================
*/
void Mem_UpdateAllocStats( int size ) {
  Mem_UpdateStats( mem_frame_allocs, size );
  Mem_UpdateStats( mem_total_allocs, size );
}

/*
==================
Mem_UpdateFreeStats
==================
*/
void Mem_UpdateFreeStats( int size ) {
  Mem_UpdateStats( mem_frame_frees, size );
  mem_total_allocs.num--;
  mem_total_allocs.totalSize -= size;
}

/*
==================
Mem_Alloc
==================
*/
void *Mem_Alloc( const int size ) {
  if ( !size ) {
    return NULL;
  }
  if ( !mem_heap ) {
#ifdef CRASH_ON_STATIC_ALLOCATION
    *((int*)0x0) = 1;
#endif
    return malloc( size );
  }
  void *mem = mem_heap->Allocate( size );
  Mem_UpdateAllocStats( mem_heap->Msize( mem ) );
  return mem;
}

/*
==================
Mem_Free
==================
*/
void Mem_Free( void *ptr ) {
  if ( !ptr ) {
    return;
  }
  if ( !mem_heap ) {
#ifdef CRASH_ON_STATIC_ALLOCATION
    *((int*)0x0) = 1;
#endif
    free( ptr );
    return;
  }
  Mem_UpdateFreeStats( mem_heap->Msize( ptr ) );
  mem_heap->Free( ptr );
}

/*
==================
Mem_Alloc16
==================
*/
void *Mem_Alloc16( const int size ) {
  if ( !size ) {
    return NULL;
  }
  if ( !mem_heap ) {
#ifdef CRASH_ON_STATIC_ALLOCATION
    *((int*)0x0) = 1;
#endif
    return malloc( size );
  }
  void *mem = mem_heap->Allocate16( size );
  // make sure the memory is 16 unsigned char aligned
  assert( ( ((int)mem) & 15) == 0 );
  return mem;
}

/*
==================
Mem_Free16
==================
*/
void Mem_Free16( void *ptr ) {
  if ( !ptr ) {
    return;
  }
  if ( !mem_heap ) {
#ifdef CRASH_ON_STATIC_ALLOCATION
    *((int*)0x0) = 1;
#endif
    free( ptr );
    return;
  }
  // make sure the memory is 16 unsigned char aligned
  assert( ( ((int)ptr) & 15) == 0 );
  mem_heap->Free16( ptr );
}

/*
==================
Mem_ClearedAlloc
==================
*/
void *Mem_ClearedAlloc( const int size ) {
  void *mem = Mem_Alloc( size );
  
  if (!mem)
    return NULL;
  
  memset( mem, 0, size );
  return mem;
}

/*
==================
Mem_ClearedAlloc
==================
*/
void Mem_AllocDefragBlock( void ) {
  mem_heap->AllocDefragBlock();
}

/*
==================
Mem_CopyString
==================
*/
char *Mem_CopyString( const char *in ) {
  char  *out;
  
  out = (char *)Mem_Alloc( strlen(in) + 1 );
  strcpy( out, in );
  return out;
}

/*
==================
Mem_Init
==================
*/
void Mem_Init( void ) {
  mem_heap = new idHeap;
  Mem_ClearFrameStats();
}

/*
==================
Mem_Shutdown
==================
*/
void Mem_Shutdown( void ) {
  idHeap *m = mem_heap;
  mem_heap = NULL;
  delete m;
}

/*
==================
Mem_EnableLeakTest
==================
*/
void Mem_EnableLeakTest( const char *name ) {
}
