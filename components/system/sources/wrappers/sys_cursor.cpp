#include "shared.h"

using namespace common;
using namespace syslib;

/*
    Описание реализации курсора
*/

struct WindowCursor::Impl: public xtl::reference_counter
{
  stl::string        name;   //имя курсора
  Platform::cursor_t handle; //дескриптор курсора
  
  Impl () : handle (0) {}
  
  Impl (const char* file_name, int hotspot_x, int hotspot_y)
    : name (file_name)
    , handle (Platform::CreateCursor (file_name, hotspot_x, hotspot_y))
  {
  }
  
  ~Impl ()
  {
    if (!handle)
      return;

    Platform::DestroyCursor (handle);
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

WindowCursor::WindowCursor ()
  : impl (new Impl)
{
}

WindowCursor::WindowCursor (const char* file_name, int hotspot_x, int hotspot_y)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
    impl = new Impl (file_name, hotspot_x, hotspot_y);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowCursor::WindowCursor");
    throw;
  }
}

WindowCursor::WindowCursor (const WindowCursor& cursor)
  : impl (cursor.impl)
{
  addref (impl);
}

WindowCursor::~WindowCursor ()
{
  release (impl);
}

WindowCursor& WindowCursor::operator = (const WindowCursor& cursor)
{
  WindowCursor (cursor).Swap (*this);

  return *this;
}

/*
    Имя курсора
*/

const char* WindowCursor::Name () const
{
  return impl->name.c_str ();
}

/*
    Получение низкоуровневого дескриптора курсора
*/

const void* WindowCursor::Handle () const
{
  return impl->handle;
}

/*
    Обмен
*/

void WindowCursor::Swap (WindowCursor& cursor)
{
  stl::swap (impl, cursor.impl);
}

namespace syslib
{

void swap (WindowCursor& cursor1, WindowCursor& cursor2)
{
  cursor1.Swap (cursor2);
}

}
