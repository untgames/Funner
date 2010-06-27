#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::iphone;

namespace
{

#if TARGET_IPHONE_SIMULATOR
  const char* OPENGL_DYLIB_PATH = "/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator4.0.sdk/System/Library/Frameworks/OpenGLES.framework/OpenGLES";
#else
  const char* OPENGL_DYLIB_PATH = "/System/Library/Frameworks/OpenGLES.framework/OpenGLES";
#endif

}

/*
    Описание реализации библиотеки функций OpenGL
*/

struct Library::Impl
{
  syslib::DynamicLibrary opengl_dylib; //библиотека OpenGL

  Impl ()
    : opengl_dylib (OPENGL_DYLIB_PATH)
    {}
};

/*
   Конструктор / деструктор
*/

Library::Library ()
  : impl (new Impl)
{
}

Library::~Library ()
{
}

/*
   Получение адреса точки входа
*/

const void* Library::GetProcAddress (const char* name, size_t search_flags)
{
  static const char* METHOD_NAME = "render::low_level::opengl::iphone::Library::GetProcAddress";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  void* proc_address = 0;

  if (search_flags & (EntrySearch_Library | EntrySearch_Context))
    proc_address = impl->opengl_dylib.GetSymbol (name);

  if (search_flags & EntrySearch_NoThrow)
    return proc_address;

  if (!proc_address)
    throw xtl::format_operation_exception (METHOD_NAME, "Entry '%s' not found in library '%s'", name, impl->opengl_dylib.Name ());

  return proc_address;
}
