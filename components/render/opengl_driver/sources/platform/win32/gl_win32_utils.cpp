#include "shared.h"

namespace
{

//получение строки с сообщением об ошибке
stl::string get_error_message (DWORD error_code)
{
  void* buffer = 0;

  size_t buffer_len = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                     0, error_code, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, 0);                 

  if (!buffer)
  {
    return common::format ("Win32 error %u", error_code);
  }
  else
  {    
      //отсечение завершающих \n и пробелов      

    char* iter = (char*)buffer;    
    
    iter [buffer_len] = '\0';    

    iter += buffer_len;

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

namespace render
{

namespace low_level
{

namespace opengl
{

namespace windows
{

//генерация исключения
void raise_error (const char* source)
{
  DWORD error_code = GetLastError ();

  if (error_code)
    throw xtl::format_operation_exception (source, "%s", get_error_message (error_code).c_str ());

  throw xtl::format_operation_exception (source, "Operation failed");
}

///Проверка поддержки расширений
stl::string get_wgl_extensions_string (const WglExtensionEntries& wgl_extension_entries, HDC device_context)
{
  stl::string extensions;
    
  if (wgl_extension_entries.GetExtensionsStringARB)
    extensions += wgl_extension_entries.GetExtensionsStringARB (device_context);
    
  if (wgl_extension_entries.GetExtensionsStringEXT)
    extensions += wgl_extension_entries.GetExtensionsStringEXT ();

  return extensions;
}

bool has_extension (const char* extensions_string, const char* extension)
{
  return extensions_string && extension && strstr (extensions_string, extension);
}

}

}

}

}
