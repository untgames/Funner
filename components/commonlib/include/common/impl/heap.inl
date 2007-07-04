inline void* operator new (size_t size)
{
  return common::MemoryManager::Allocate (size);
}

inline void* operator new [] (size_t size)
{
  return common::MemoryManager::Allocate (size);
}

inline void operator delete (void* p,size_t)
{
  common::MemoryManager::Deallocate (p);
}

inline void operator delete [] (void* p,size_t)
{
  common::MemoryManager::Deallocate (p);
}
