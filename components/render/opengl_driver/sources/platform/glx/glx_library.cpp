#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    Описание реализации библиотеки
*/

struct Library::Impl
{
///Конструктор
  Impl ()
  {
  }
};

/*
    Конструктор / деструктор
*/

Library::Library ()  
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Library::Library");
    throw;
  }
}

Library::~Library ()
{
}

/*
    Получение адреса точки входа
*/

const void* Library::GetProcAddress (const char* name, size_t search_flags)
{
  static const char* METHOD_NAME = "render::low_level::opengl::glx::Library::GetProcAddress";
  
  if (!name)
  {
    if (search_flags & EntrySearch_NoThrow)
      return 0;

    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
  }

  const void* address = 0;

  if (!address && (search_flags & EntrySearch_Context))
    address = (void*)glXGetProcAddress ((GLubyte*)name);

  if (address)
    return address;

  if (search_flags & EntrySearch_NoThrow)
    return 0;

  throw xtl::format_operation_exception (METHOD_NAME, "OpenGL entry '%s' not found in GLX", name);
}
