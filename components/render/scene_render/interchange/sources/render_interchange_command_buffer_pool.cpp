#include "shared.h"

using namespace render::scene::interchange;

namespace
{

/*
    Константы
*/

size_t DEFAULT_BUFFER_SIZE   = 65536; //размер буфера по умолчанию
size_t DEFAULT_BUFFERS_COUNT = 8;     //резервируемое количество буферов

}

/*
    Описание реализации пула буферов команд
*/

typedef stl::vector<CommandBuffer> BufferArray;

struct CommandBufferPool::Impl: public xtl::reference_counter, public xtl::spin_lock
{  
  typedef xtl::lock_ptr<Impl> Guard;

  size_t      default_buffer_size; //размер буфера по умолчанию
  BufferArray buffers;             //буферы

/// Конструктор
  Impl () : default_buffer_size (DEFAULT_BUFFER_SIZE)
  {
    buffers.reserve (DEFAULT_BUFFERS_COUNT);
  }

/// Очистка неиспользуемых буферов
  void FlushUnusedBuffers ()
  {
    if (buffers.size () <= DEFAULT_BUFFERS_COUNT)
      return;

    for (BufferArray::iterator iter=buffers.begin (), end=buffers.end (); iter!=end;)
      if (iter->UseCount () == 1)
      {
        buffers.erase (iter);

        end = buffers.end ();
      }
      else ++iter;
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

CommandBufferPool::CommandBufferPool ()
  : impl (new Impl)
{
}

CommandBufferPool::CommandBufferPool (const CommandBufferPool& pool)
  : impl (pool.impl)
{
}

CommandBufferPool::~CommandBufferPool ()
{
  release (impl);
}

CommandBufferPool& CommandBufferPool::operator = (const CommandBufferPool& pool)
{
  CommandBufferPool tmp = pool;

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    Резервируемый размер буфера по умолчанию
*/

void CommandBufferPool::SetDefaultBufferSize (size_t size)
{
  Impl::Guard guard (impl);

  impl->default_buffer_size = size;  
}

size_t CommandBufferPool::DefaultBufferSize () const
{
  Impl::Guard guard (impl);

  return impl->default_buffer_size; 
}

/*
    Создание буфера
*/

CommandBuffer CommandBufferPool::CreateBuffer ()
{
  try
  {
    Impl::Guard guard (impl);

      //попытка найти буфер среди уже созданных

    for (BufferArray::iterator iter=impl->buffers.begin (), end=impl->buffers.end (); iter!=end; ++iter)
      if (iter->UseCount () == 1)
      {
        CommandBuffer result = *iter;

        result.Clear ();

        result.Reserve (impl->default_buffer_size);

        impl->FlushUnusedBuffers ();

        return result;
      }

      //создание нового буфера

    CommandBuffer buffer;

    buffer.Reserve (impl->default_buffer_size);

    impl->buffers.push_back (buffer);

    return buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::CommandBufferPool::CreateBuffer");
    throw;
  }
}
