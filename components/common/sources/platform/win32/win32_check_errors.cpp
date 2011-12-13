#include "shared.h"

namespace common
{

namespace
{

//получение строки с сообщением об ошибке
stl::string get_error_message (DWORD error_code)
{
  void* buffer = 0;

  FormatMessageW (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                 0, error_code, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buffer, 0, 0);                 

  if (!buffer)
  {
    return common::format ("Win32 error %u", error_code);
  }
  else
  {    
      //отсечение завершающих \n и пробелов

    wchar_t* iter = (wchar_t*)buffer;
    
    iter += xtl::xstrlen (iter);    
    
    if (iter == buffer)
      return "";      
      
    for (bool loop=true; loop;)
      switch (*--iter)
      {
        case '\n':
        case '\r':
        case ' ':
        case '\t':
          break;
        default:
        {
          iter [1] = '\0';
          loop     = false;
          break;
        }
      }

    stl::string message = common::format ("Win32 error %u. %s", error_code, common::tostring((const wchar_t*)buffer).c_str ());

    LocalFree (buffer);

    return message;
  }
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
