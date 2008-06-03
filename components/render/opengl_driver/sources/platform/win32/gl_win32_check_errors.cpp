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
//MAKELANGID (LANG_NEUTRAL,SUBLANG_DEFAULT)                     

      if (!buffer)
        common::raise_invalid_operation ("render::low_level::opengl::Win32ErrorMessage::Win32ErrorMessage", "Internal error at FormatMessage");
        
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

//генерация исключения
void raise_error (const char* source)
{
  DWORD error_code = GetLastError ();
  
  if (error_code)
    common::raise_invalid_operation (source, Win32ErrorMessage (GetLastError ()).Message ());
  
  common::raise_invalid_operation (source, "Operation failed");
}

}

}

}
