#include "shared.h"

using namespace network;

namespace
{

// ласс инициализирующий и прекращающий работу с WinSock 2 dll
class WinSockInitializer
{
  public:
    WinSockInitializer ()
    {
      WSADATA wsa_data;

      int result = WSAStartup (MAKEWORD(2, 2), &wsa_data);

      if (result)
        throw xtl::format_operation_exception ("network::WinSockInitializer", "Error while calling ::WSAStartup: '%s'", get_error_message (result).c_str ());
    }

    ~WinSockInitializer ()
    {
      WSACleanup ();
    }
};

typedef common::Singleton<WinSockInitializer> WinSockInitializerSingleton;

}

namespace network
{

void init_networking ()
{
  WinSockInitializerSingleton::Instance ();
}

}


/*
   ѕолучение ip-адреса по имени хоста
*/

void Win32Platform::GetAddressByHostName (const char* host_name, size_t& address_size, unsigned char (&address) [16])
{
  try
  {
    if (!host_name)
      throw xtl::make_null_argument_exception ("", "host_name");

    init_networking ();

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
