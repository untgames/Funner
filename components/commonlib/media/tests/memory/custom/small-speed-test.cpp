#include <common/heap.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>

#pragma warning (disable : 4018 4101)

#define USE_DL_PREFIX
#include "third-party/dlmalloc.c"
#include "third-party/doom3malloc.cpp"

using namespace common;

enum Test
{
  TEST_STDMALLOC,
  TEST_DLMALLOC,
  TEST_MYMALLOC,
  TEST_DOOM3MALLOC
};

const size_t TOTAL          = 4000000;
const size_t BUF_SIZE       = 10000;
const size_t MIN_DUMMY_SIZE = 400;
const size_t MIN_BLOCK_SIZE = 100+MIN_DUMMY_SIZE+8*BUF_SIZE;
const size_t MAX_BLOCK_SIZE = MIN_BLOCK_SIZE+BUF_SIZE*8;

size_t block_size [BUF_SIZE];
size_t block_index [BUF_SIZE];
void*  mem_buf [BUF_SIZE];
Heap   heap;  

template <Test test> struct TestAllocator {};

template <Test test> void* allocate (size_t size);
template <Test test> void  deallocate (size_t size);

template <Test test>
size_t TestSpeed ()
{
  memset (mem_buf,0,sizeof (mem_buf));
  
  for (size_t i=0;i<BUF_SIZE;i++)
    mem_buf [i] = TestAllocator<test>::allocate (MIN_DUMMY_SIZE+i*16);
    
  for (size_t i=0;i<BUF_SIZE;i+=2)
  {
    size_t index = i;
    
    if (mem_buf [index])
    {
      TestAllocator<test>::deallocate (mem_buf [index]);
      mem_buf [index] = NULL;
    }
  }

  clock_t start = clock ();  
    
  for (size_t i=0;i<TOTAL;i++)  
  {
    size_t index = i & (BUF_SIZE-1), 
           size  = block_size [block_index [index]];
    
    if (mem_buf [index])
      TestAllocator<test>::deallocate (mem_buf [index]);

    mem_buf [index] = TestAllocator<test>::allocate (size);
  }
  
  clock_t end = clock ();    
  
  for (size_t i=0;i<BUF_SIZE;i++)
    if (mem_buf [i])
    {
      TestAllocator<test>::deallocate (mem_buf [i]);
      mem_buf [i] = NULL;
    }

  return end-start;
}

template <> struct TestAllocator<TEST_STDMALLOC> 
{
  static void* allocate   (size_t size) { return malloc (size); }
  static void  deallocate (void* p)     { free (p); }
};

template <> struct TestAllocator<TEST_DLMALLOC> 
{
  static void* allocate   (size_t size) { return dlmalloc (size); }
  static void  deallocate (void* p)     { dlfree (p); }
};

template <> struct TestAllocator<TEST_DOOM3MALLOC> 
{
  static void* allocate   (size_t size) { return Mem_Alloc (size); }
  static void  deallocate (void* p)     { Mem_Free (p); }
};

template <> struct TestAllocator<TEST_MYMALLOC> 
{
  static void* allocate   (size_t size) { return heap.TryAllocate (size); }
  static void  deallocate (void* p)     { heap.Deallocate (p); }
};

int main ()
{
  printf ("Results of small blocks allocation speed test:\n");  
  
  printf ("Max block size: %u\n",MAX_BLOCK_SIZE);
  
  for (size_t i=0;i<BUF_SIZE;i++)
  {
    block_size  [i] = MIN_BLOCK_SIZE + rand () % (MAX_BLOCK_SIZE-MIN_BLOCK_SIZE);
    block_index [i] = rand () % BUF_SIZE;
//    block_size  [i] = MIN_BLOCK_SIZE + i % (MAX_BLOCK_SIZE-MIN_BLOCK_SIZE);
//    block_index [i] = BUF_SIZE-i-1;
  }
  
  heap.SetGranularity (4*1024*1024,64*1024*1024);
  heap.SetReserveSize (1024*1024*1024,1024*1024*1024);
  
  dlmallopt (M_GRANULARITY,64*1024*1024);  
  dlmallopt (M_TRIM_THRESHOLD,1024*1024*1024);
  
//  printf ("std-malloc speed:  %.1fM\n",float (TOTAL)/float (TestSpeed<TEST_STDMALLOC> ())*float (CLOCKS_PER_SEC)/1000000.0f);
//  printf ("doom3malloc speed: %.1fM\n",float (TOTAL)/float (TestSpeed<TEST_DOOM3MALLOC> ())*float (CLOCKS_PER_SEC)/1000000.0f);    
  printf ("my-malloc speed:   %.1fM\n",float (TOTAL)/float (TestSpeed<TEST_MYMALLOC> ())*float (CLOCKS_PER_SEC)/1000000.0f);    
  printf ("dlmalloc speed:    %.1fM\n",float (TOTAL)/float (TestSpeed<TEST_DLMALLOC> ())*float (CLOCKS_PER_SEC)/1000000.0f);  
  
  HeapStat stat;
  
  heap.GetStatistics (stat);
  
  printf ("Statistics:\n");
  printf ("\tAlloc from system:       %.2fM\n",float (stat.sys_allocate_size)/1000000.0f);
  printf ("\tDeallocate to system:    %.2fM\n",float (stat.sys_deallocate_size)/1000000.0f);  
  printf ("\tSystem alloc calls:      %u\n",stat.sys_allocate_count);
  printf ("\tSystem free calls:       %u\n",stat.sys_deallocate_count);
  printf ("\tReserve size:            %.2fM\n",float (stat.reserve_size)/1000000.0f);
  printf ("\tAllocate count:          %u\n",stat.allocate_count);
  printf ("\tDeallocate count:        %u\n",stat.deallocate_count);
  printf ("\tAllocate size:           %.2fM\n",float (stat.allocate_size)/1000000.0f);
  printf ("\tDeallocate size:         %.2fM\n",float (stat.deallocate_size)/1000000.0f);
  printf ("\tMedium search:           %u\n",stat.medium_search_count);
  printf ("\tMedium top use count:    %u\n",stat.medium_top_use_count);  
  printf ("\tMedium search steps:     %u\n",stat.medium_search_steps_count);  
  printf ("\tMedium max search steps: %u\n",stat.medium_max_search_steps_count);
  printf ("\tAvg medium steps:        %.2f\n",float (stat.medium_search_steps_count)/float (stat.medium_search_count));
  printf ("\tMin block size:          %u\n",stat.min_block_size);
  printf ("\tMax block size:          %u\n",stat.max_block_size);    

  return 0;
}
