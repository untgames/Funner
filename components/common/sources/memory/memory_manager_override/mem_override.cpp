#include <common/heap.h>

using namespace common;

void* operator new (size_t size)
{
  return MemoryManager::Allocate (size);
}

void* operator new [] (size_t size)
{
  return MemoryManager::Allocate (size);
}

void operator delete (void* p)
{
  MemoryManager::Deallocate (p);
}

void operator delete [] (void* p)
{
  MemoryManager::Deallocate (p);
}
