#include <platform/platform.h>
#include <common/exception.h>

using namespace syslib;

/*
    Генерация исключения: работа с динамическими библиотеками невозможна для платформы по умолчанию
*/

namespace
{

void Raise (const char* method_name)
{
  common::RaiseNotSupported (method_name, "No dynamic libraries support for default platform");
}

}

/*
    Загрузка динамических библиотек
*/

Platform::dll_t Platform::LoadLibrary (const wchar_t* name)
{
  Raise ("syslib::DefaultPlatform::LoadLibrary");

  return 0;
}

void Platform::UnloadLibrary (dll_t)
{
  Raise ("syslib::DefaultPlatform::UnloadLibrary");
}

void* Platform::GetSymbol (dll_t, const char*)
{
  Raise ("syslib::DefaultPlatform::GetSymbol");
  return 0;
}
