#include "shared.h"

using namespace common;

ICustomAllocator* DefaultPlatform::GetSystemAllocator ()
{
  return Singleton<MallocAllocator>::InstancePtr ();
}

ICustomFileSystem* DefaultPlatform::GetFileSystem ()
{
  return Singleton<StdioFileSystem>::InstancePtr ();
}

ICustomFileSystem* DefaultPlatform::GetIOSystem ()
{
  return Singleton<StdioIOSystem>::InstancePtr ();
}
