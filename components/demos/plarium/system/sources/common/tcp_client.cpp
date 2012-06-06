#include "shared.h"

using namespace plarium::system;
using namespace plarium::utility;

namespace
{

#ifdef _MSC_VER

//получение строки с сообщением об ошибке
std::string get_winsock_error_message (DWORD error_code)
{
  void* buffer = 0;

  FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                 0, error_code, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, 0);

  if (!buffer)
  {
    return format ("Win32 error %u", error_code);
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

    std::string message = format ("Win32 error %u. %s", error_code, buffer);

    LocalFree (buffer);

    return message;
  }
}

void raise_host_error (const char* source, const char* function)
{
  DWORD error_code = WSAGetLastError ();

  throw std::runtime_error (format ("%s - error at %s - %s", source, function, get_winsock_error_message (error_code).c_str ()));
}

void raise_error (const char* source, const char* function)
{
  raise_host_error (source, function);
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
        throw std::runtime_error (format ("WinSockInitializer - Error while calling ::WSAStartup: '%s'", get_winsock_error_message (result).c_str ()));
    }

    ~WinSockInitializer ()
    {
      WSACleanup ();
    }
};

int close_socket (SOCKET socket)
{
  return closesocket (socket) == SOCKET_ERROR;
}

#else

void raise_error (const char* source, const char* function)
{
  throw std::runtime_error (format ("%s - error at %s - %s", source, function, strerror (errno)));
}

void raise_host_error (const char* source, const char* function)
{
  throw std::runtime_error (format ("%s - error at %s - %s", source, function, hstrerror (h_errno)));
}

int close_socket (int socket)
{
  return close (socket);
}

#endif

}

struct TcpClient::Impl
{
#ifdef _MSC_VER
  SOCKET              socket;     //socket descriptor
#else
  int                 socket;     //socket descriptor
#endif
  bool                connected;  //was connected
  ITcpClientListener* listener;   //listener

  Impl ()
    : socket (0)
    , connected (false)
    , listener (0)
    {}
};

/*
   Constructor / destructor
*/

TcpClient::TcpClient ()
  : impl (new Impl)
  {}

TcpClient::~TcpClient ()
{
  try
  {
    if (IsConnected ())
      Disconnect ();
  }
  catch (std::exception& e)
  {
    printf ("Can't close socket, exception '%s'\n", e.what ());
  }
  catch (...)
  {
    printf ("Can't close socket, unknown exception\n");
  }

  delete impl;
}

/*
   Connection
*/

void TcpClient::Connect (const char* host, unsigned short port)
{
#ifdef _MSC_VER
  static WinSockInitializer network_init;
#endif

  if (IsConnected ())
    throw std::logic_error ("TcpClient::Connect - Socket already connected");

  if (!host)
    throw std::invalid_argument ("TcpClient::Connect - null host");

  hostent *entry = gethostbyname (host);

  if (!entry)
    raise_host_error ("TcpClient::Connect", "::gethostbyname");

  if (!entry->h_addr_list [0])
    throw std::runtime_error ("TcpClient::Connect - error at ::gethostbyname, null address list returned");

  short            address_family;
  sockaddr_storage address_storage;
  size_t           address_storage_size;

  switch (entry->h_length)
  {
    case 4:
    {
      address_family = AF_INET;

      address_storage_size = sizeof (sockaddr_in);

      sockaddr_in* address_desc = (sockaddr_in*)&address_storage;

      address_desc->sin_port = htons (port);

      address_desc->sin_addr.s_addr = 0;

      address_desc->sin_addr.s_addr |= entry->h_addr_list [0][0];
      address_desc->sin_addr.s_addr |= (unsigned char)entry->h_addr_list [0][1] << 8;
      address_desc->sin_addr.s_addr |= (unsigned char)entry->h_addr_list [0][2] << 16;
      address_desc->sin_addr.s_addr |= (unsigned char)entry->h_addr_list [0][3] << 24;

      break;
    }
    case 16:
    {
      address_family = AF_INET6;

      address_storage_size = sizeof (sockaddr_in6);

      sockaddr_in6* address_desc = (sockaddr_in6*)&address_storage;

      address_desc->sin6_port = htons (port);

      for (size_t i = 0; i < 16; i++)
        address_desc->sin6_addr.s6_addr [i] = entry->h_addr_list [0][i];

      break;
    }
    default:
      throw std::runtime_error (format ("TcpClient::Connect - host name '%s' resolved, but host address is unsupported.", host));
  }

  address_storage.ss_family = address_family;

  impl->socket = socket (address_family, SOCK_STREAM, IPPROTO_TCP);

#ifdef _MSC_VER
  if (impl->socket == INVALID_SOCKET)
    raise_error ("TcpClient::Connect", "::socket");
#else
  if (impl->socket < 0)
    raise_error ("TcpClient::Connect", "::socket");
#endif

  try
  {
#ifdef _MSC_VER
    WSAEVENT dummy_event = WSACreateEvent ();

    if (dummy_event == WSA_INVALID_EVENT)
      raise_error ("TcpClient::Connect", "::WSACreateEvent");

    if (WSAEventSelect (impl->socket, dummy_event, 0) == SOCKET_ERROR)
      raise_error ("TcpClient::Connect", "::WSAEventSelect");

    u_long state_value = 0;

    if (ioctlsocket (impl->socket, FIONBIO, &state_value) == SOCKET_ERROR)
      raise_error ("TcpClient::Connect", "::ioctlsocket");
#else
    int flags = fcntl (impl->socket, F_GETFL, 0);

    if (flags < 0)
      raise_error ("TcpClient::Connect", "::fcntl");

    flags &= ~O_NONBLOCK;

    if (fcntl (impl->socket, F_SETFL, flags) == -1)
      raise_error ("TcpClient::Connect", "::fcntl");
#endif

    if (connect (impl->socket, (sockaddr*)&address_storage, address_storage_size))
      raise_error ("TcpClient::Connect", "::connect");
  }
  catch (...)
  {
    if (close_socket (impl->socket))
      printf ("Can't close socket, error '%s'\n", strerror (errno));

    throw;
  }

  impl->connected = true;
}

void TcpClient::Disconnect ()
{
  if (!IsConnected ())
    return;

  impl->connected = false;

  if (close_socket (impl->socket))
    throw std::runtime_error (format ("Can't close socket, error '%s'\n", strerror (errno)));
}

/*
   Data sending
*/

void TcpClient::Send (const void* buffer, size_t size)
{
  if (!IsConnected ())
    throw std::logic_error ("TcpClient::Send - Socket is not connected");

  const char* data_pointer = (const char*)buffer;

  while (size)
  {
    int sent_bytes = send (impl->socket, data_pointer, size, 0);

    if (sent_bytes < 0)
    {
      if (errno == ECONNRESET)
      {
        try
        {
          Disconnect ();
        }
        catch (...)
        {
        }

        if (impl->listener)
          impl->listener->OnDisconnect (*this);

        throw std::runtime_error ("TcpClient::Send - A connection was forcibly closed by a peer.");
      }
      else
        raise_error ("TcpClient::Connect", "::send");
    }

    size         -= sent_bytes;
    data_pointer += sent_bytes;
  }
}

/*
   Data receiving
*/

void TcpClient::Receive (void* buffer, size_t size)
{
  if (!IsConnected ())
    throw std::logic_error ("TcpClient::Receive - Socket is not connected");

  char* data_pointer = (char*)buffer;

  while (size)
  {
    int received_bytes = recv (impl->socket, data_pointer, size, 0);

    if ((received_bytes < 0 && errno == ECONNRESET) || !received_bytes)
    {
      try
      {
        Disconnect ();
      }
      catch (...)
      {
      }

      if (impl->listener)
        impl->listener->OnDisconnect (*this);

      throw std::runtime_error ("TcpClient::Receive - A connection was forcibly closed by a peer.");
    }

    if (received_bytes < 0)
      raise_error ("TcpClient::Connect", "::recv");

    size         -= received_bytes;
    data_pointer += received_bytes;
  }
}

/*
   Socket state
*/

bool TcpClient::IsConnected () const
{
  return impl->connected;
}

/*
   Event listening
*/

void TcpClient::SetListener (ITcpClientListener* listener)
{
  impl->listener = listener;
}

ITcpClientListener* TcpClient::Listener () const
{
  return impl->listener;
}
