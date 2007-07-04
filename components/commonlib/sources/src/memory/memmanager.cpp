#include <common/heap.h>
#include <common/singleton.h>

using namespace common;

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
  return MemoryManagerSingleton::Instance ().heap;
}

void MemoryManager::Init ()
{
  MemoryManagerSingleton::Init ();
}

void MemoryManager::Shutdown ()
{
  MemoryManagerSingleton::Destroy ();
}

void MemoryManager::Restart ()
{
  MemoryManagerSingleton::Destroy ();
  MemoryManagerSingleton::Init ();
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
  void* p = MemoryManagerSingleton::Instance ().heap.Allocate (size);
  
  if (!p)
    throw std::bad_alloc ();
    
  return p;
}

void MemoryManager::Deallocate (void* p)
{
  if (p && MemoryManagerSingleton::IsInitialized ())
    MemoryManagerSingleton::Instance ().heap.Deallocate (p);
}
