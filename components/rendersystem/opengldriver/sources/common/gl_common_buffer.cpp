#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Размер буфера
*/

size_t Buffer::GetSize ()
{
  BufferDesc desc;
  
  GetDesc (desc);
  
  return desc.size;
}

/*
    Копирование данных между буферами
*/

void Buffer::CopyTo (size_t source_offset, size_t size, IBuffer& destination, size_t destination_offset)
{
  size_t source_size      = GetSize (),
         destination_size = destination.GetSize ();

  if (source_offset >= source_size || destination_offset >= destination_size)
    return;
    
  if (source_size - source_offset < size)
    size = source_size - source_offset;    
  
  if (destination_size - destination_offset < size)
    size = destination_size - destination_offset;

  CopyToImpl (source_offset, size, destination, destination_offset);
}

void Buffer::CopyToImpl (size_t source_offset, size_t size, IBuffer& destination, size_t destination_offset)
{
  xtl::uninitialized_storage<char> temp_storage (size);
  
  GetData             (source_offset, size, temp_storage.data ());
  destination.SetData (destination_offset, size, temp_storage.data ());
}
