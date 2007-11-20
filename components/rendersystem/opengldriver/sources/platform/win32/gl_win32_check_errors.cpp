#include "shared.h"

namespace
{

//буфер сообщения о Win32 ошибке
class Win32ErrorMessage
{
  public:
    Win32ErrorMessage (DWORD error_code) : buffer (0)
    {
      FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                     0, error_code, 0, (LPTSTR)&buffer, 0, 0);

      if (!buffer)
        common::RaiseInvalidOperation ("render::low_level::opengl::Win32ErrorMessage::Win32ErrorMessage", "Internal error at FormatMessage");
        
        //отсечение завершающих \n и пробелов
        
      char* iter = (char*)buffer;
      
      iter += strlen (iter);
      
      if (iter == buffer)
        return;
      
      for (;;)
        switch (*--iter)
        {
          case '\n':
          case '\r':
          case ' ':
          case '\t':
            break;
          default:
            iter [1] = '\0';
            return;
        }
    }

    ~Win32ErrorMessage ()
    {
      if (buffer)
        LocalFree (buffer);
    }

    const char* Message () const { return reinterpret_cast<const char*> (buffer); }

  private:
    void* buffer;
};

}

namespace render
{

namespace low_level
{

namespace opengl
{

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source)
{ 
  DWORD error_code = GetLastError ();
  
  if (error_code)
    common::RaiseInvalidOperation (source, Win32ErrorMessage (GetLastError ()).Message ());
}

//генерация исключения
void raise_error (const char* source)
{
  check_errors (source);
  
  common::RaiseInvalidOperation (source, "Operation failed");
}

}

}

}
