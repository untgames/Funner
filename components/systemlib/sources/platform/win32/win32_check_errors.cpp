#include "shared.h"

namespace syslib
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
        throw WinAPIException ("system::Win32ErrorMessage::Win32ErrorMessage", "Internal error at FormatMessage");
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

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void CheckErrors (const char* source)
{ 
  DWORD error_code = GetLastError ();
  
  if (error_code)
    throw WinAPIException (source, Win32ErrorMessage (GetLastError ()).Message ());
}

}
