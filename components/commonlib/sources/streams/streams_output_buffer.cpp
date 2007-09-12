#include "shared.h"

using namespace common;

/*
    Описание реализации OutputStreamBuffer
*/

struct OutputStreamBuffer::Impl
{
  StreamBuffer  buffer;   //буфер
  WriteFunction writer;   //функция записи
  size_t        position; //позиция в буфере (количество записанных байт)
  
  size_t Available () const { return buffer.Size () - position; }
  
  Impl (size_t buffer_size, const WriteFunction& in_writer = &DefaultWriter) : writer (in_writer), position (0)
  {
    buffer.Resize (buffer_size);
  }
};

/*
    Конструкторы / деструктор
*/

OutputStreamBuffer::OutputStreamBuffer (size_t buffer_size)  
  : impl (new Impl (buffer_size))
{
}

OutputStreamBuffer::OutputStreamBuffer (const WriteFunction& writer, size_t buffer_size)
  : impl (new Impl (buffer_size, writer))
{
}

OutputStreamBuffer::~OutputStreamBuffer ()
{
  try
  {
    Flush ();
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Управление размером буфера
*/

size_t OutputStreamBuffer::Size () const
{
  return impl->buffer.Size ();
}

void OutputStreamBuffer::Resize (size_t buffer_size)
{
  Flush ();
  impl->buffer.Resize (buffer_size, impl->position);
}

/*
    Присоединение пользовательского буфера
*/

void OutputStreamBuffer::SetUserBuffer (void* buffer, size_t size)
{
  Flush ();
  
  impl->position = 0;
  
  impl->buffer.SetUserBuffer (buffer, size);
}

/*
    Функция записи данных
*/

void OutputStreamBuffer::SetWriter (const WriteFunction& in_writer)
{
  Flush ();
  
  impl->position = 0;
  impl->writer   = in_writer;
}

const OutputStreamBuffer::WriteFunction& OutputStreamBuffer::Writer () const
{
  return impl->writer;
}

/*
    Функция записи данных по умолчанию
*/

size_t OutputStreamBuffer::DefaultWriter (const void*, size_t)
{
  return 0;
}

/*
    Запись
*/

size_t OutputStreamBuffer::Write (const void* buffer, size_t size)
{
  size_t        write_size    = 0;
  const char*   ptr           = static_cast<const char*> (buffer);
  StreamBuffer& stream_buffer = impl->buffer;

    //если буферизация отключена - пишем в обход буфера

  if (!stream_buffer.Size ())
    return impl->writer (buffer, size);

  while (size)
  {   
      //если буфер заполнен - сбрасываем его

    if (!impl->Available ())
    {
      Flush ();

      if (!impl->Available ())
        return write_size;
    }

    size_t block_size = size;

    if (block_size > impl->Available ())
      block_size = impl->Available ();

    memcpy ((char*)stream_buffer.Data () + impl->position, ptr, block_size);

    impl->position += block_size;
    ptr            += block_size;
    write_size     += block_size;
    size           -= block_size;
  }
  
  return write_size;
}

/*
    Сброс буфера
*/

void OutputStreamBuffer::Flush ()
{ 
  if (!impl->position)
    return;    
    
  size_t write_size = impl->writer (impl->buffer.Data (), impl->position);
  
  impl->position -= write_size;  
   
  if (impl->position && write_size)
    memcpy (impl->buffer.Data (), (char*)impl->buffer.Data () + write_size, impl->position);
}

/*
    Обмен
*/

void OutputStreamBuffer::Swap (OutputStreamBuffer& osb)
{
  stl::swap (impl, osb.impl);
}

namespace common
{

void swap (OutputStreamBuffer& osb1, OutputStreamBuffer& osb2)
{
  osb1.Swap (osb2);
}

}
