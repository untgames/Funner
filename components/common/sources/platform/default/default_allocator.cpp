#include "shared.h"

using namespace common;

void* MallocAllocator::Allocate (size_t size)
{
  return malloc (size);
}

void MallocAllocator::Deallocate (void* p)
{
  free (p);
}
