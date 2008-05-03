#include "shared.h"

using namespace common;

/*
    Конструктор / деструктор
*/

StreamBuffer::StreamBuffer ()
  : is_user_buffer (false), buffer (0), buffer_size (0), visible_size (0)
  {}

StreamBuffer::~StreamBuffer ()
{
  if (!is_user_buffer)
    ::operator delete (buffer);
}

/*
    Установка пользовательского буфера
*/

void StreamBuffer::SetBuffer (void* in_buffer, size_t in_buffer_size, bool in_is_user_buffer)
{
  if (!is_user_buffer)
    ::operator delete (buffer);
    
  buffer         = in_buffer;
  buffer_size    = in_buffer_size;
  visible_size   = in_buffer_size;
  is_user_buffer = in_is_user_buffer;
}

void StreamBuffer::SetUserBuffer (void* in_buffer, size_t in_buffer_size)
{
  if (!in_buffer && in_buffer_size)
    RaiseNullArgument ("common::StreamBuffer::SetUserBuffer", "buffer");

  SetBuffer (in_buffer, in_buffer_size, in_buffer != 0);
}

/*
    Изменение размера буфера
*/

void StreamBuffer::Resize (size_t size, size_t initialized_size)
{
  if (size == visible_size)
    return;
  
  if (size < buffer_size)
  {
    visible_size = size;

    return;
  }

  if (is_user_buffer)
  {
    visible_size = buffer_size;    
  }
  else
  {
    char* new_buffer = (char*)::operator new (size);

    if (buffer)
      memcpy (new_buffer, buffer, initialized_size < visible_size ? initialized_size : visible_size);

    SetBuffer (new_buffer, size, false);
  }
}
