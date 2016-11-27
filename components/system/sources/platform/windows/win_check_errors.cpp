#include "shared.h"

namespace syslib
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

    LocalFree (buffer);

    return message;
  }
}

//получение ошибки COM
stl::string get_com_error_message (HRESULT result)
{
  return _com_error (result).ErrorMessage ();
}

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source)
{
  DWORD error_code = GetLastError ();
  
  if (error_code)
    throw xtl::format_operation_exception ("syslib::check_errors", get_error_message (error_code).c_str ());
}

void raise_error (const char* source)
{
  try
  {
    check_errors (source);
  }
  catch (xtl::exception& e)
  {
    e.touch (source);
    throw;
  }

  throw xtl::format_operation_exception (source, "Invalid operation");
}

void raise_com_error (const char* source, const char* message, HRESULT result)
{
  try
  {
    _com_error error (result);

    throw xtl::format_operation_exception ("syslib::raise_com_error", "%s. %s", message, error.ErrorMessage ());
  }
  catch (xtl::exception& e)
  {
    e.touch (source);
    throw;
  }
}

}
