#include <common/heap.h>
#include <common/singleton.h>

using namespace common;

void* operator new (size_t size)
{
  return common::MemoryManager::Allocate (size);
}

void* operator new [] (size_t size)
{
  return common::MemoryManager::Allocate (size);
}

void operator delete (void* p)
{
  common::MemoryManager::Deallocate (p);
}

void operator delete [] (void* p)
{
  common::MemoryManager::Deallocate (p);
}

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации менеджера памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MemoryManagerImpl
{
  Heap heap;  
};

}

typedef Singleton<MemoryManagerImpl> MemoryManagerSingleton;

/*
    MemoryManager
*/

Heap& MemoryManager::GetHeap ()
{
  return MemoryManagerSingleton::Instance ()->heap;
}

bool MemoryManager::IsInitialized ()
{
  return MemoryManagerSingleton::IsInitialized ();
}

/*
    Врапперы для упрощения выделения памяти из основного менеджера    
*/

void* MemoryManager::Allocate (size_t size)
{
  void* p = MemoryManagerSingleton::Instance ()->heap.Allocate (size);
  
  if (!p)
    throw std::bad_alloc ();
    
  return p;
}

void* MemoryManager::Allocate (size_t size, size_t align, size_t offset)
{
  void* p = MemoryManagerSingleton::Instance ()->heap.Allocate (size, align, offset);

  if (!p)
    throw std::bad_alloc ();

  return p;
}

void MemoryManager::Deallocate (void* p)
{  
  if (p && MemoryManagerSingleton::IsInitialized ())
    MemoryManagerSingleton::Instance ()->heap.Deallocate (p);
}
