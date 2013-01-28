#include "shared.h"

using namespace Platform;

namespace
{

__declspec (thread) int winrt_initializer_counter = 0;

}

namespace syslib
{

/*
    Инициализатор WinRT
*/

/// Конструктор
WinRtInitializer::WinRtInitializer ()
{
  try
  {
    if (!winrt_initializer_counter)
    {
      static const int WINRT_INIT_MULTITHREADED = 0x1;

      HRESULT result = Windows::Foundation::Initialize ((RO_INIT_TYPE)WINRT_INIT_MULTITHREADED);

      if (result != S_OK)
        raise_com_error ("Windows::Foundation::Initialize", result);

      printf ("%p\n", setlocale (LC_ALL, "Russian"));

//      Windows::Globalization::ApplicationLanguages::PrimaryLanguageOverride = "en";
    }

    winrt_initializer_counter++;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::windows::WinRtInitializer::WinRtInitializer");
    throw;
  }
}

/// Деструктор
WinRtInitializer::~WinRtInitializer ()
{
  --winrt_initializer_counter;

  if (!winrt_initializer_counter)
    Windows::Foundation::Uninitialize ();
}

/*
    Утилиты
*/

/// Преобразование строки
stl::string tostring (String^ input)
{
  if (input == nullptr)
    return stl::string ();

  const wchar_t* data   = input->Data ();
  size_t         length = input->Length ();

  if (!length || !data)
    return stl::string ();

  stl::wstring wstr (data, length);

  return common::tostring (wstr);
}

/// Выброс исключения с сообщением HRESULT
void raise_com_error (const char* method, HRESULT error)
{
  throw xtl::format_operation_exception (method, _com_error (error).ErrorMessage ());
}

}
