#include "shared.h"

using namespace common;

/*
    Описание реализации InputStreamBuffer
*/

struct InputStreamBuffer::Impl
{
  StreamBuffer buffer;   //буфер
  ReadFunction reader;   //функция чтения
  size_t       position; //позиция в буфере чтения
  size_t       finish;   //конец буфера чтения
  
  size_t Available () const { return finish - position; }
  
  Impl (size_t buffer_size, const ReadFunction& in_reader = &DefaultReader) : reader (in_reader), position (0), finish (0)
  {
    buffer.Resize (buffer_size);
  }
};

/*
    Конструкторы / деструктор
*/

InputStreamBuffer::InputStreamBuffer (size_t buffer_size)
  : impl (new Impl (buffer_size))
{
}

InputStreamBuffer::InputStreamBuffer (const ReadFunction& reader, size_t buffer_size)
  : impl (new Impl (buffer_size, reader))
{
}

InputStreamBuffer::~InputStreamBuffer ()
{
}

/*
    Управление размером буфера
*/

size_t InputStreamBuffer::Size () const
{
  return impl->buffer.Size ();
}

void InputStreamBuffer::Resize (size_t buffer_size)
{
  if (buffer_size < impl->position)
    buffer_size = impl->position;
   
  impl->buffer.Resize (buffer_size, impl->finish);
}

/*
    Чтение
*/

size_t InputStreamBuffer::Read (void* buffer, size_t size)
{
  size_t        read_size     = 0;
  char*         ptr           = static_cast<char*> (buffer);
  StreamBuffer& stream_buffer = impl->buffer;

    //если буферизация отключена - читаем в обход буфера

  if (!stream_buffer.Size ())
    return impl->reader (buffer, size);

  while (size)
  {
      //если буфер опустошен - заполняем его

    if (!impl->Available ())
    {
      impl->position = 0;
      impl->finish   = impl->reader (stream_buffer.Data (), stream_buffer.Size ());      
      
      if (!impl->Available ())
        return read_size;
    }

    size_t block_size = size;

    if (block_size > impl->Available ())
      block_size = impl->Available ();

    memcpy (ptr, (char*)stream_buffer.Data () + impl->position, block_size);


    impl->position += block_size;
    ptr            += block_size;
    read_size      += block_size;
    size           -= block_size;
  }
  
  return read_size;
}

/*
    Присоединение пользовательского буфера
*/

void InputStreamBuffer::SetUserBuffer (void* buffer, size_t size)
{
  impl->buffer.SetUserBuffer (buffer, size);
  
  impl->position = 0;
  impl->finish   = size;
}

/*
    Функция чтения данных
*/

void InputStreamBuffer::SetReader (const ReadFunction& reader)
{
  impl->reader = reader;
}

const InputStreamBuffer::ReadFunction& InputStreamBuffer::Reader () const
{
  return impl->reader;
}

/*
    Функция чтения данных по умолчанию
*/

size_t InputStreamBuffer::DefaultReader (void*, size_t)
{
  return 0;
}

/*
    Обмен
*/

void InputStreamBuffer::Swap (InputStreamBuffer& isb)
{  
  swap (impl, isb.impl);
}

namespace common
{

void swap (InputStreamBuffer& isb1, InputStreamBuffer& isb2)
{
  isb1.Swap (isb2);
}

}
