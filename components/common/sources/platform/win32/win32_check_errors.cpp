#include "shared.h"

namespace common
{

namespace
{

const size_t MAX_BUFFER_SIZE = 2048;

//получение строки с сообщением об ошибке
stl::string get_error_message (DWORD error_code)
{
  void* buffer = malloc (MAX_BUFFER_SIZE);

  if (!buffer)
    return common::format ("Win32 error %u", error_code);

  DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

  FormatMessageW (flags, 0, error_code, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buffer, 0, 0);

    //отсечение завершающих \n и пробелов

  wchar_t* iter = (wchar_t*)buffer;
  
  iter += xtl::xstrlen (iter);    
  
  if (iter == buffer)
    return "";      
    
  for (bool loop=true; loop;)
    switch (*--iter)
    {
      case L'\n':
      case L'\r':
      case L' ':
      case L'\t':
        break;
      default:
      {
        iter [1] = L'\0';
        loop     = false;
        break;
      }
    }

  stl::string message = common::format ("Win32 error %u. %s", error_code, common::tostring((const wchar_t*)buffer).c_str ());

  free (buffer);

  return message;
}

}

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source)
{
  DWORD error_code = GetLastError ();
  
  if (error_code)
    throw xtl::format_operation_exception ("common::check_errors", get_error_message (error_code).c_str ());
}

void raise_error (const char* source)
{
  check_errors (source);

  throw xtl::format_operation_exception (source, "Invalid operation");
}

}
