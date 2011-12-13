#include <common/heap.h>

#include <stdio.h>
#include <cstdlib>

const size_t MAX_SIZE   = 1024*1024;
const size_t ALIGN_SIZE = 8;

using namespace common;

inline bool check_align (void* p,size_t align)
{
  return (((size_t)p) & (align-1)) == 0;
}

int main ()
{
  printf ("Results of align2_test:\n");

  Heap heap;

  size_t allocated_memory = 0;

  for (size_t i = 0; i < 1000; i++)
  {
    size_t block_size = rand () % MAX_SIZE;

    void* p = heap.Allocate (block_size);

    if (!p)
    {
      printf ("memory ended after allocating %u bytes\n", allocated_memory);
      break;
    }

    allocated_memory += block_size;

    if (!check_align (p, ALIGN_SIZE))
      printf ("align failed at size=%u, pointer is %p\n", block_size, p);
  }

  printf ("success\n");

  return 0;
}
