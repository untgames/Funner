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
      RaiseNullArgument ("", "name");

    HMODULE library = LoadLibraryW (name);
  
    if (!library)
      raise_error (common::format ("::LoadLibraryW('%S')", name).c_str ());
      
    return (dll_t)library;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("syslib::Win32Platform::LoadLibrary");
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
      RaiseNullArgument ("", "library");

    FreeLibrary (library);

    check_errors ("::UnloadLibrary");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("syslib::Win32Platform::UnloadLibrary");
    throw;
  }
}

void* Platform::GetSymbol (dll_t handle, const char* symbol_name)
{
  static const char* METHOD_NAME = "syslib::Win32Platform::GetSymbol";

  HMODULE library = (HMODULE)handle;

  if (!library)
    RaiseNullArgument ("", "library");

  if (!symbol_name)
    RaiseNullArgument ("", "symbol_name");

  void* address = GetProcAddress (library, symbol_name);
  
    //сбрасываем все ошибки

  SetLastError (0);    

  return address;
}
