#include <syslib/dll.h>

#include <common/exception.h>
#include <common/utf_converter.h>

#include <stl/string>

#include <platform/platform.h>

using namespace syslib;
using namespace common;

/*
    Описание реализации динамической библиотеки
*/

struct DynamicLibrary::Impl
{
  Platform::dll_t handle;       //дескриптор библиотеки
  stl::string     name;         //имя библиотеки
  stl::wstring    name_unicode; //unicode имя библиотеки
  
  Impl () : handle (0) {}
};

/*
    Конструкторы / деструктор
*/

DynamicLibrary::DynamicLibrary ()
  : impl (new Impl)
{
}

DynamicLibrary::DynamicLibrary (const char* name)
  : impl (new Impl)
{
  try
  {
    if (!name)
      RaiseNullArgument ("", "name");

    impl->name         = name;
    impl->name_unicode = towstring (name);
    impl->handle       = Platform::LoadLibrary (impl->name_unicode.c_str ());
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("syslib::DynamicLibrary::DynamicLibrary(const char*)");
    throw;
  }
}

DynamicLibrary::DynamicLibrary (const wchar_t* name)
  : impl (new Impl)
{
  try
  {
    if (!name)
      RaiseNullArgument ("", "name");

    impl->name_unicode = name;
    impl->name         = tostring (name);
    impl->handle       = Platform::LoadLibrary (impl->name_unicode.c_str ());
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("syslib::DynamicLibrary::DynamicLibrary(const wchar_t*)");
    throw;
  }
}

DynamicLibrary::~DynamicLibrary ()
{
}

/*
    Загрузка / выгрузка
*/

void DynamicLibrary::Load (const char* name)
{
  DynamicLibrary (name).Swap (*this);
}

void DynamicLibrary::Load (const wchar_t* name)
{
  DynamicLibrary (name).Swap (*this);
}

void DynamicLibrary::Unload ()
{
  DynamicLibrary ().Swap (*this);
}

//загружена ли библиотека
bool DynamicLibrary::IsLoaded () const
{
  return impl->handle != 0;
}

/*
    Имя библиотеки
*/

const char* DynamicLibrary::Name () const
{
  return impl->name.c_str ();
}

const wchar_t* DynamicLibrary::NameUnicode () const
{
  return impl->name_unicode.c_str ();
}

/*
    Получение адреса символа по строковому идентификатору
*/

void* DynamicLibrary::GetSymbol (const char* name) const
{
  try
  {
    if (!name || !impl->handle)
      return 0;

    return Platform::GetSymbol (impl->handle, name);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("syslib::DynamicLibrary::GetSymbol");
    throw;
  }
}

/*
    Обмен
*/

void DynamicLibrary::Swap (DynamicLibrary& library)
{
  impl.swap (library.impl);
}

namespace syslib
{

void swap (DynamicLibrary& library1, DynamicLibrary& library2)
{
  library1.Swap (library2);
}

}
