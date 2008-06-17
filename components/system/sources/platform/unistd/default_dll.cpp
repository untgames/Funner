#include "shared.h"

using namespace syslib;

/*
    Генерация исключения: работа с динамическими библиотеками невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No dynamic libraries support for default platform");
}

}

/*
    Загрузка динамических библиотек
*/

Platform::dll_t Platform::LoadLibrary (const wchar_t* name)
{
  raise ("syslib::DefaultPlatform::LoadLibrary");

  return 0;
}

void Platform::UnloadLibrary (dll_t)
{
  raise ("syslib::DefaultPlatform::UnloadLibrary");
}

void* Platform::GetSymbol (dll_t, const char*)
{
  raise ("syslib::DefaultPlatform::GetSymbol");
  return 0;
}
