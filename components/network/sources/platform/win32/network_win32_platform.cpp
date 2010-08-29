#include "shared.h"

using namespace network;

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

void raise_error (const char* source, int error_code)
{
  throw xtl::format_operation_exception (source, get_error_message (error_code).c_str ());
}

//Класс инициализирующий и прекращающий работу с WinSock 2 dll
class WinSockInitializer
{
  public:
    WinSockInitializer ()
    {
      WSADATA wsa_data;

      int result = WSAStartup (MAKEWORD(2, 2), &wsa_data);

      if (result)
        raise_error ("::WSAStartup", result);
    }

    ~WinSockInitializer ()
    {
      WSACleanup ();
    }
};

typedef common::Singleton<WinSockInitializer> WinSockInitializerSingleton;

}

/*
   Получение ip-адреса по имени хоста
*/

void Win32Platform::GetAddressByHostName (const char* host_name, size_t& address_size, unsigned char (&address) [16])
{
  try
  {
    if (!host_name)
      throw xtl::make_null_argument_exception ("", "host_name");

    WinSockInitializerSingleton::Instance ();

    hostent *entry = gethostbyname (host_name);

    if (!entry)
      check_errors ("::gethostbyname");

    if ((size_t)entry->h_length > sizeof (address))
      throw xtl::format_operation_exception ("", "Host name '%s' resolved, but host address is too long.", host_name);

    if (!entry->h_addr_list [0])
      throw xtl::format_operation_exception ("", "Error at ::gethostbyname, null address list returned");

    address_size = entry->h_length;

    for (size_t i = 0; i < address_size; i++)
      address [i] = entry->h_addr_list [0][i];
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::Win32Platform::GetAddressByHostName");
    throw;
  }
}
