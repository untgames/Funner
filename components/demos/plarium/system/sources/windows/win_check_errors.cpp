#include "shared.h"

using namespace plarium;

namespace
{

//получение строки с сообщением об ошибке
std::string get_error_message (DWORD error_code)
{
  void* buffer = 0;

  FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                 0, error_code, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, 0);

  if (!buffer)
  {
    return utility::format ("Win32 error %u", error_code);
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

    std::string message = utility::format ("Win32 error %u. %s", error_code, buffer);

    LocalFree (buffer);

    return message;
  }
}

}

namespace plarium
{

namespace system
{

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source)
{
  DWORD error_code = GetLastError ();
  
  if (error_code)
    throw std::runtime_error (utility::format ("common::check_errors", get_error_message (error_code).c_str ()));
}

void raise_error (const char* source)
{
  check_errors (source);

  throw std::runtime_error (source);
}

}

}
