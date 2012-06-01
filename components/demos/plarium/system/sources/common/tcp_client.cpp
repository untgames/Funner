#include "shared.h"

using namespace plarium::system;
using namespace plarium::utility;

namespace
{

void raise_error (const char* failed_function)
{
  throw std::runtime_error (format ("%s failed, error '%s'", failed_function, strerror (errno)));
}

}

struct TcpClient::Impl
{
  int                 socket;     //socket descriptor
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
      Close ();
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
  if (IsConnected ())
    throw std::logic_error ("TcpClient::Connect - Socket already connected");

  if (!host)
    throw std::invalid_argument ("TcpClient::Connect - null host");

  hostent *entry = gethostbyname (host);

  if (!entry)
    throw std::runtime_error (format ("TcpClient::Connect - can't resolve host name '%s'. Error '%s' while calling ::gethostbyname", host, hstrerror (h_errno)));

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

  if (socket < 0)
    raise_error ("TcpClient::Connect - ::socket");

  try
  {
    int flags = fcntl (impl->socket, F_GETFL, 0);

    if (flags < 0)
      raise_error ("TcpClient::Connect - ::fcntl");

    flags &= ~O_NONBLOCK;

    if (fcntl (impl->socket, F_SETFL, flags) == -1)
      raise_error ("TcpClient::Connect - ::fcntl");

    if (connect (impl->socket, (sockaddr*)&address_storage, address_storage_size))
      raise_error ("TcpClient::Connect - ::connect");
  }
  catch (...)
  {
    if (close (impl->socket))
      printf ("Can't close socket, error '%s'\n", strerror (errno));

    throw;
  }

  impl->connected = true;
}

void TcpClient::Close ()
{
  if (!IsConnected ())
    throw std::logic_error ("TcpClient::Close - Socket is not connected");

  impl->connected = false;

  if (close (impl->socket))
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
        Close ();

        if (impl->listener)
          impl->listener->OnDisconnect (*this);

        throw std::runtime_error ("TcpClient::Send - A connection was forcibly closed by a peer.");
      }
      else
        raise_error ("TcpClient::Connect - ::send");
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
      Close ();

      if (impl->listener)
        impl->listener->OnDisconnect (*this);

      throw std::runtime_error ("TcpClient::Receive - A connection was forcibly closed by a peer.");
    }

    if (received_bytes < 0)
      raise_error ("TcpClient::Connect - ::recv");

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
