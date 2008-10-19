#include "shared.h"

using namespace syslib;
using namespace common;

/*
    Загрузка динамических библиотек
*/

#undef LoadLibrary

Platform::dll_t Platform::LoadLibrary (const wchar_t* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    HMODULE library = LoadLibraryW (name);

    if (!library)
      raise_error (common::format ("::LoadLibraryW('%S')", name).c_str ());

    return (dll_t)library;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::LoadLibrary");
    throw;
  }

  return 0;
}

void Platform::UnloadLibrary (dll_t handle)
{
  try
  {
    HMODULE library = (HMODULE)handle;

    if (!library)
      throw xtl::make_null_argument_exception ("", "library");

    FreeLibrary (library);

    check_errors ("::UnloadLibrary");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::UnloadLibrary");
    throw;
  }
}

void* Platform::GetSymbol (dll_t handle, const char* symbol_name)
{
  HMODULE library = (HMODULE)handle;

  if (!library)
    throw xtl::make_null_argument_exception (METHOD_NAME, "library");

  if (!symbol_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "symbol_name");

  void* address = (void*)GetProcAddress (library, symbol_name);

    //сбрасываем все ошибки

  SetLastError (0);

  return address;
}
