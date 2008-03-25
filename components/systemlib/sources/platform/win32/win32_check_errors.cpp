#include "shared.h"

#include <locale.h>

namespace syslib
{

namespace
{

//получение строки с сообщением об ошибке
stl::string get_error_message (DWORD error_code)
{
  void* buffer = 0;

  FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                 0, error_code, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, 0);

  if (!buffer)
  {
    return common::format ("Win32 error %u", error_code);
  }
  else
  {    
      //отсечение завершающих \n и пробелов

    char* iter = (char*)buffer;
    
    iter += strlen (iter);    
    
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

    stl::string message = common::format ("Win32 error %u. %s", error_code, buffer);    

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
    throw WinAPIException (source, get_error_message (GetLastError ()).c_str ());
}

void raise_error (const char* source)
{
  check_errors (source);
  
  throw WinAPIException (source, "Invalid operation");
}

}
