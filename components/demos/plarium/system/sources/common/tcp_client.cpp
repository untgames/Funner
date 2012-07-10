#include "shared.h"

using namespace plarium::system;
using namespace plarium::utility;

namespace
{

//Error codes
#ifdef _MSC_VER
const int CONNECTION_RESET_ERROR = WSAECONNRESET;
const int WOULD_BLOCK_ERROR      = WSAEWOULDBLOCK;
const int EAGAIN_ERROR           = WSAETIMEDOUT;
#else
const int CONNECTION_RESET_ERROR = ECONNRESET;
const int WOULD_BLOCK_ERROR      = EWOULDBLOCK;
const int EAGAIN_ERROR           = EAGAIN;
#endif

#ifdef _MSC_VER

//получение строки с сообщением об ошибке
sgi_stl::string get_winsock_error_message (DWORD error_code)
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

    sgi_stl::string message = format ("Win32 error %u. %s", error_code, buffer);

    LocalFree (buffer);

    return message;
  }
}

void raise_host_error (const char* source, const char* function)
{
  DWORD error_code = WSAGetLastError ();

  throw sgi_stl::runtime_error (format ("%s - error at %s - %s", source, function, get_winsock_error_message (error_code).c_str ()));
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
        throw sgi_stl::runtime_error (format ("WinSockInitializer - Error while calling ::WSAStartup: '%s'", get_winsock_error_message (result).c_str ()));
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
  throw sgi_stl::runtime_error (format ("%s - error at %s - %s", source, function, strerror (errno)));
}

void raise_host_error (const char* source, const char* function)
{
  throw sgi_stl::runtime_error (format ("%s - error at %s - %s", source, function, hstrerror (h_errno)));
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

  template <class T>
  void SetSocketOption (int option, const T& value, int level = SOL_SOCKET)
  {
    if (setsockopt (socket, level, option, (const char*)&value, sizeof (T)))
      raise_error ("TcpClient::SetSocketOption", "setsockopt");
  }

  void SetBlockingMode (bool state)
  {
#ifdef _MSC_VER
    WSAEVENT dummy_event = WSACreateEvent ();

    if (dummy_event == WSA_INVALID_EVENT)
      raise_error ("TcpClient::SetBlockingMode", "::WSACreateEvent");

    if (WSAEventSelect (socket, dummy_event, 0) == SOCKET_ERROR)
      raise_error ("TcpClient::SetBlockingMode", "::WSAEventSelect");

    u_long state_value = state ? 0 : 1;

    if (ioctlsocket (socket, FIONBIO, &state_value) == SOCKET_ERROR)
      raise_error ("TcpClient::SetBlockingMode", "::ioctlsocket");
#else
    int flags = fcntl (socket, F_GETFL, 0);

    if (flags < 0)
      raise_error ("TcpClient::SetBlockingMode", "::fcntl");

    if (state)
      flags &= ~O_NONBLOCK;
    else
      flags |= O_NONBLOCK;

    if (fcntl (socket, F_SETFL, flags) == -1)
      raise_error ("TcpClient::SetBlockingMode", "::fcntl");
#endif
  }
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
    throw sgi_stl::logic_error ("TcpClient::Connect - Socket already connected");

  if (!host)
    throw sgi_stl::invalid_argument ("TcpClient::Connect - null host");

  hostent *entry = gethostbyname (host);

  if (!entry)
    raise_host_error ("TcpClient::Connect", "::gethostbyname");

  if (!entry->h_addr_list [0])
    throw sgi_stl::runtime_error ("TcpClient::Connect - error at ::gethostbyname, null address list returned");

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
      throw sgi_stl::runtime_error (format ("TcpClient::Connect - host name '%s' resolved, but host address is unsupported.", host));
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
    impl->SetBlockingMode (true);

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
    throw sgi_stl::runtime_error (format ("Can't close socket, error '%s'\n", strerror (errno)));
}

/*
   Data sending (returns amoutn of sent data)
*/

size_t TcpClient::Send (const void* buffer, size_t size, size_t timeout_in_milliseconds)
{
  if (!IsConnected ())
    throw sgi_stl::logic_error ("TcpClient::Send - Socket is not connected");

  if (!buffer)
    throw sgi_stl::invalid_argument ("TcpClient::Send - null buffer");

#ifdef _MSC_VER
  impl->SetSocketOption<DWORD> (SO_SNDTIMEO, (DWORD)timeout_in_milliseconds);
#else
  timeval timeout;

  timeout.tv_sec  = timeout_in_milliseconds / 1000;
  timeout.tv_usec = (timeout_in_milliseconds % 1000) * 1000;

  impl->SetSocketOption<timeval> (SO_SNDTIMEO, timeout);
#endif

  int sent_bytes = send (impl->socket, (const char*)buffer, size, 0);

  if (sent_bytes < 0)
  {
    int error_code =
#ifdef _MSC_VER
        WSAGetLastError ();
#else
        errno;
#endif

    if (error_code == EAGAIN_ERROR || errno == WOULD_BLOCK_ERROR)
      return 0;

    if (error_code == CONNECTION_RESET_ERROR)
      throw sgi_stl::runtime_error ("TcpClient::Send - A connection was forcibly closed by a peer.");
    else
      raise_error ("TcpClient::Send", "::send");
  }

  return sent_bytes;
}

/*
   Data receiving (returns amoutn of received data)
*/

size_t TcpClient::Receive (void* buffer, size_t size, size_t timeout_in_milliseconds)
{
  if (!IsConnected ())
    throw sgi_stl::logic_error ("TcpClient::Receive - Socket is not connected");

  if (!buffer)
    throw sgi_stl::invalid_argument ("TcpClient::Receive - null buffer");

#ifdef _MSC_VER
  impl->SetSocketOption<DWORD> (SO_RCVTIMEO, (DWORD)timeout_in_milliseconds);
#else
  timeval timeout;

  timeout.tv_sec  = timeout_in_milliseconds / 1000;
  timeout.tv_usec = (timeout_in_milliseconds % 1000) * 1000;

  impl->SetSocketOption<timeval> (SO_RCVTIMEO, timeout);
#endif

  int received_bytes = recv (impl->socket, (char*)buffer, size, 0);

  int error_code =
#ifdef _MSC_VER
      WSAGetLastError ();
#else
      errno;
#endif

  if ((received_bytes < 0 && error_code == CONNECTION_RESET_ERROR) || !received_bytes)
    throw sgi_stl::runtime_error ("TcpClient::Receive - A connection was forcibly closed by a peer.");

  if (received_bytes < 0)
  {
    if (error_code == EAGAIN_ERROR || errno == WOULD_BLOCK_ERROR)
      return 0;
    else
      raise_error ("TcpClient::Receive", "::recv");
  }

  return received_bytes;
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
