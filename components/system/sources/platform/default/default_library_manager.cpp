#include "shared.h"

using namespace syslib;

/*
    Генерация исключения: работа с динамическими библиотеками невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No dynamic libraries support for this platform");
}

}

/*
    Загрузка динамических библиотек
*/

dll_t DefaultLibraryManager::LoadLibrary (const wchar_t* name)
{
  raise ("syslib::DefaultLibraryManager::LoadLibrary");

  return 0;
}

void DefaultLibraryManager::UnloadLibrary (dll_t)
{
  raise ("syslib::DefaultLibraryManager::UnloadLibrary");
}

void* DefaultLibraryManager::GetSymbol (dll_t, const char*)
{
  raise ("syslib::DefaultLibraryManager::GetSymbol");

  return 0;
}
