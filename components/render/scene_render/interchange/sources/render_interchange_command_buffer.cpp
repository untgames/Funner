#include "shared.h"

using namespace render::scene::interchange;

/*
    Описание реализации буфера команд
*/

typedef xtl::uninitialized_storage<char> Buffer;

struct CommandBuffer::Impl: public xtl::reference_counter
{
  Buffer buffer; //буфер с данными

  static Impl* GetDefault ()
  {
    return &xtl::singleton_default<Impl, true>::instance ();
  }

  void Reserve (size_t new_size, bool copy_data)
  {
    if (new_size <= buffer.capacity ())
      return;

    size_t new_up_size = buffer.capacity () * 2;

    if (new_up_size > new_size)
      new_size = new_up_size;

    static const size_t MAX_SIZE = ~0u;

    if (new_size < buffer.capacity ())
      new_size = MAX_SIZE;

    buffer.reserve (new_size, copy_data);
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

CommandBuffer::CommandBuffer ()
  : impl (Impl::GetDefault ())
{
  addref (impl);
}

CommandBuffer::CommandBuffer (const CommandBuffer& buffer)
  : impl (buffer.impl)
{
  addref (impl);
}

CommandBuffer::~CommandBuffer ()
{
  release (impl);
}

CommandBuffer& CommandBuffer::operator = (const CommandBuffer& buffer)
{
  CommandBuffer (buffer).Swap (*this);

  return *this;
}

/*
    Счетчик ссылок
*/

size_t CommandBuffer::UseCount () const
{
  return impl->use_count ();
}

/*
    Размер буфера
*/

size_t CommandBuffer::Size () const
{
  return impl->buffer.size ();
}

/*
    Доступ к данным
*/

const void* CommandBuffer::Data () const
{
  return impl->buffer.data ();
}

void* CommandBuffer::Data ()
{
  return impl->buffer.data ();
}

/*
    Изменение размера
*/

void CommandBuffer::Resize (size_t new_size, bool copy_data)
{
  if (impl == Impl::GetDefault ())
    impl = new Impl;

  impl->Reserve (new_size, copy_data);  

  impl->buffer.resize (new_size, copy_data);
}

/*
    Резервирование размера
*/

size_t CommandBuffer::Capacity () const
{
  return impl->buffer.capacity ();
}

void CommandBuffer::Reserve (size_t new_size)
{
  if (impl == Impl::GetDefault ())
    impl = new Impl;

  impl->Reserve (new_size, true);
}

/*
    Обмен
*/

void CommandBuffer::Swap (CommandBuffer& buffer)
{
  stl::swap (impl, buffer.impl);
}

namespace render {
namespace scene {
namespace interchange {

void swap (CommandBuffer& buffer1, CommandBuffer& buffer2)
{
  buffer1.Swap (buffer2);
}

}}}
