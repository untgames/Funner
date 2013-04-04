#include "shared.h"

using namespace Platform;

namespace syslib
{

namespace win8
{

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

}
