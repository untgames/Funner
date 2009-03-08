#include <dlfcn.h>

#include <xtl/common_exceptions.h>

#include <common/strlib.h>
#include <common/utf_converter.h>

#include <platform/platform.h>

using namespace syslib;

namespace
{

void raise_error (const char* source)
{
  const char* error = dlerror ();

  if (error)
    throw xtl::format_operation_exception (source, error);

  throw xtl::format_operation_exception (source, "Unknown error");
}

}
/*
    Загрузка динамических библиотек
*/

Platform::dll_t Platform::LoadLibrary (const wchar_t* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    size_t dlopen_flags = RTLD_NOW | RTLD_GLOBAL;

    void* library = dlopen (common::tostring (name).c_str (), dlopen_flags);

    if (!library)
      raise_error (common::format ("::dlopen('%S', %lu)", name, dlopen_flags).c_str ());

    return (dll_t)library;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::UnistdPlatform::LoadLibrary");
    throw;
  }

  return 0;
}

void Platform::UnloadLibrary (dll_t library)
{
  try
  {
    if (!library)
      throw xtl::make_null_argument_exception ("", "library");

    if (dlclose (library))
      raise_error ("::dlclose");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::UnistdPlatform::UnloadLibrary");
    throw;
  }
}

void* Platform::GetSymbol (dll_t library, const char* symbol_name)
{
  static const char* METHOD_NAME = "syslib::UnistdPlatform::GetSymbol";

  if (!library)
    throw xtl::make_null_argument_exception (METHOD_NAME, "library");

  if (!symbol_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "symbol_name");

  void* address = dlsym (library, symbol_name);

  return address;
}
