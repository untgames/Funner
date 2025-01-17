#include <dlfcn.h>

#include <xtl/common_exceptions.h>

#include <common/strlib.h>
#include <common/utf_converter.h>

#include <shared/platform.h>

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

dll_t UnistdLibraryManager::LoadLibrary (const wchar_t* name)
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
    exception.touch ("syslib::UnistdLibraryManager::LoadLibrary");
    throw;
  }

  return 0;
}

void UnistdLibraryManager::UnloadLibrary (dll_t library)
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
    exception.touch ("syslib::UnistdLibraryManager::UnloadLibrary");
    throw;
  }
}

void* UnistdLibraryManager::GetSymbol (dll_t library, const char* symbol_name)
{
  static const char* METHOD_NAME = "syslib::UnistdLibraryManager::GetSymbol";

  if (!library)
    throw xtl::make_null_argument_exception (METHOD_NAME, "library");

  if (!symbol_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "symbol_name");

  void* address = dlsym (library, symbol_name);

  return address;
}
