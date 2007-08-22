#include <memory.h>
#include "shared.h"

using namespace medialib::geometry;

/*
    MeshBuffer
*/

MeshBuffer::MeshBuffer ()
  : start (0), finish (0), end_of_storage (0)
  {}
  
MeshBuffer::MeshBuffer (size_t size)
  : start (0), finish (0), end_of_storage (0)
{
  Resize (size);
}

MeshBuffer::MeshBuffer (const MeshBuffer& MeshBuffer)
{
  Resize (MeshBuffer.Size ());
  
  memcpy (start, MeshBuffer.start, finish - start);
}

MeshBuffer::~MeshBuffer ()
{
  if (start)
    ::operator delete (start);
}

void MeshBuffer::Resize (size_t new_size)
{
  size_t size = finish - start;
  
  if (new_size == size)
    return;

  if (new_size > size)
    Reserve (new_size);

  finish = start + new_size;
}

void MeshBuffer::Reserve (size_t new_size)
{
  size_t size = end_of_storage - start;

  if (new_size <= size)
    return;
    
  char* new_buffer = (char*)::operator new (new_size);
  
  memcpy (new_buffer, start, finish - start);
  
  if (start)
    ::operator delete (start);
    
  finish         = new_buffer + (finish - start);
  start          = new_buffer;  
  end_of_storage = new_buffer + new_size;
}
