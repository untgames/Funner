#include <common/heap.h>

using namespace common;

void* operator new (size_t size)
{
  return MemoryManager::Allocate (size);
}

void* operator new (std::size_t size, const std::nothrow_t& tag) noexcept
{
  return MemoryManager::Allocate (size);
}

void* operator new [] (size_t size)
{
  return MemoryManager::Allocate (size);
}

void* operator new [] (std::size_t size, const std::nothrow_t& tag) noexcept
{
  return MemoryManager::Allocate (size);
}

void operator delete (void* p) noexcept
{
  MemoryManager::Deallocate (p);
}

void operator delete (void* p, const std::nothrow_t& tag) noexcept
{
  MemoryManager::Deallocate (p);
}

void operator delete [] (void* p) noexcept
{
  MemoryManager::Deallocate (p);
}

void operator delete [] (void* p, const std::nothrow_t& tag) noexcept
{
  MemoryManager::Deallocate (p);
}
