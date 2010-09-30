#include "shared.h"

using namespace network;

namespace
{

const char* LOG_NAME = "network.socket";

const int LISTEN_QUEUE_SIZE = 4;

/*
   Сокет
*/

class WinSocket : public SocketImpl, public xtl::reference_counter
{
  public:
///Конструктор/деструктор
    WinSocket (int in_socket, const SocketAddress& in_remote_address)
    : socket (in_socket)
    , local_address_getted (false)
    , remote_address (in_remote_address)
    , remote_address_getted (true)
    , receive_closed (false)
    , send_closed (false)
    , connected (true)
    , bound (false)
    {}

    WinSocket (SocketDomain socket_domain, SocketProtocol protocol)
      : local_address_getted (false)
      , remote_address_getted (false)
      , receive_closed (false)
      , send_closed (false)
      , connected (false)
      , bound (false)
    {
      try
      {
        int domain, type, socket_protocol;

        switch (socket_domain)
        {
          case SocketDomain_IpV4:
            domain = AF_INET;
            break;
          case SocketDomain_IpV6:
            domain = AF_INET6;
            break;
          default:
            throw xtl::format_operation_exception ("", "Unsupported socket domain %d", socket_domain);
        }

        switch (protocol)
        {
          case SocketProtocol_Tcp:
            type            = SOCK_STREAM;
            socket_protocol = IPPROTO_TCP;
            break;
          case SocketProtocol_Udp:
            type            = SOCK_DGRAM;
            socket_protocol = IPPROTO_UDP;
            break;
          default:
            throw xtl::format_operation_exception ("", "Unsupported socket protocol %d\n", protocol);
        }

        socket = ::socket (domain, type, socket_protocol);

        if (socket == INVALID_SOCKET)
          raise_error ("::socket");

        SetBlockingMode (true);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::WinSocket");
        throw;
      }
    }

    ~WinSocket ()
    {
      if (closesocket (socket) == SOCKET_ERROR)
      {
        common::Log log (LOG_NAME);

        log.Printf ("Can't close socket, error '%s'\n", get_error_message (WSAGetLastError ()));
      }
    }

///Адреса сторон
    const SocketAddress& LocalAddress ()
    {
      if (local_address_getted)
        return local_address;

      sockaddr_storage address_storage;   //!!!!!!!!!!!WIN32 define min OS
      int              address_storage_size = sizeof (address_storage);

      try
      {
        if (getsockname (socket, (sockaddr*)&address_storage, &address_storage_size))
          raise_error ("::getsockname");

        SocketAddress local_address_value = GetSocketAddress (address_storage, address_storage_size);

        local_address.Swap (local_address_value);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::LoaclAddress");
        throw;
      }

      return local_address;
    }

    const SocketAddress& RemoteAddress ()
    {
      if (remote_address_getted)
        return remote_address;

      throw xtl::format_operation_exception ("network::WinSocket::RemoteAddress", "Can't get remote address, there was no accepted connection");
    }

///Протокол
    SocketProtocol Protocol ()
    {
      try
      {
        int socket_type = GetSocketOption<int> (SO_TYPE);

        switch (socket_type)
        {
          case SOCK_STREAM: return SocketProtocol_Tcp;
          case SOCK_DGRAM:  return SocketProtocol_Udp;
          default:
            throw xtl::format_operation_exception ("", "Can't map socket type %d to socket protocol", socket_type);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::Protocol");
        throw;
      }
    }

///Закрытие каналов передачи данных
    bool IsReceiveClosed ()
    {
      return receive_closed;
    }

    bool IsSendClosed ()
    {
      return send_closed;
    }

    void CloseReceive ()
    {
      if (receive_closed)
        return;

      try
      {
        if (shutdown (socket, SD_RECEIVE))
          raise_error ("::shutdown");
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::CloseReceive");
        throw;
      }

      receive_closed = true;
    }

    void CloseSend ()
    {
      if (send_closed)
        return;

      try
      {
        if (shutdown (socket, SD_SEND))
          raise_error ("::shutdown");
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::CloseSend");
        throw;
      }

      send_closed = true;
    }

///Соединение
    void Bind (const SocketAddress& address)
    {
      try
      {
        sockaddr_storage address_storage;
        size_t           address_storage_size;

        FillSockaddrStorage (address, address_storage, address_storage_size);

        if (bind (socket, (sockaddr*)&address_storage, address_storage_size))
          raise_error ("::bind");
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::Bind");
        throw;
      }

      bound = true;
    }

    void Connect (const SocketAddress& address, size_t timeout)
    {
      //!!!!!!!!!!TODO: timeout

      try
      {
        sockaddr_storage address_storage;
        size_t           address_storage_size;

        FillSockaddrStorage (address, address_storage, address_storage_size);

        if (connect (socket, (sockaddr*)&address_storage, address_storage_size))
          raise_error ("::connect");
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::Connect");
        throw;
      }

      connected = true;
    }

///Приём соединений
    void Listen ()
    {
      try
      {
        if (listen (socket, LISTEN_QUEUE_SIZE))
          raise_error ("::listen");
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::Listen");
        throw;
      }
    }

    SocketImpl* Accept ()
    {
      sockaddr_storage address_storage;
      int              address_storage_size = sizeof (address_storage);

      int new_socket = accept (socket, (sockaddr*)&address_storage, &address_storage_size);

      SocketAddress new_socket_remote_address;

      try
      {
        if (new_socket < 0)
          raise_error ("::accept");

        new_socket_remote_address = GetSocketAddress (address_storage, address_storage_size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::Accept");
        throw;
      }

      return new WinSocket (new_socket, new_socket_remote_address);
    }

    SocketImpl* TryAccept ()
    {
      try
      {
        SetBlockingMode (false);

        SocketImpl* return_value = 0;

        try
        {
          return_value = Accept ();
        }
        catch (...)
        {
        }

        SetBlockingMode (true);

        return return_value;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::TryAccept");
        throw;
      }
    }

///Флаги сокета
    bool IsConnected ()
    {
      return connected;
    }

    bool IsBound ()
    {
      return bound;
    }

    bool IsKeepAlive ()
    {
      try
      {
        return GetSocketOption<int> (SO_KEEPALIVE) != 0;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::IsKeepAlive");
        throw;
      }
    }

    bool IsOobInline ()
    {
      try
      {
        return GetSocketOption<int> (SO_OOBINLINE) != 0;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::IsOobInline");
        throw;
      }
    }

    bool IsReuseAddress ()
    {
      try
      {
        return GetSocketOption<int> (SO_REUSEADDR) != 0;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::IsReuseAddress");
        throw;
      }
    }

    bool IsTcpNoDelay ()
    {
      try
      {
        return GetSocketOption<int> (TCP_NODELAY, IPPROTO_TCP) != 0;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::IsTcpNoDelay");
        throw;
      }
    }

    void SetKeepAlive (bool state)
    {
      try
      {
        SetSocketOption<int> (SO_KEEPALIVE, state ? 1 : 0);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::SetKeepAlive");
        throw;
      }
    }

    void SetOobInline (bool state)
    {
      try
      {
        SetSocketOption<int> (SO_OOBINLINE, state ? 1 : 0);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::SetOobInline");
        throw;
      }
    }

    void SetReuseAddress (bool state)
    {
      try
      {
        SetSocketOption<int> (SO_REUSEADDR, state ? 1 : 0);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::SetReuseAddress");
        throw;
      }
    }

    void SetTcpNoDelay (bool state)
    {
      try
      {
        SetSocketOption<int> (TCP_NODELAY, state ? 1 : 0, IPPROTO_TCP);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::SetTcpNoDelay");
        throw;
      }
    }

///Параметры сокета
    void SetReceiveBufferSize (size_t size)
    {
      try
      {
        SetSocketOption<int> (SO_RCVBUF, size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::SetReceiveBufferSize");
        throw;
      }
    }

    void SetSendBufferSize (size_t size)
    {
      try
      {
        SetSocketOption<int> (SO_SNDBUF, size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::SetSendBufferSize");
        throw;
      }
    }

    size_t ReceiveBufferSize ()
    {
      try
      {
        return GetSocketOption<int> (SO_RCVBUF);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::ReceiveBufferSize");
        throw;
      }
    }

    size_t SendBufferSize ()
    {
      try
      {
        return GetSocketOption<int> (SO_SNDBUF);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::SendBufferSize");
        throw;
      }
    }

///Чтение / запись данных
    size_t Receive (void* buffer, size_t size, size_t timeout_in_milliseconds)
    {
      try
      {
        timeval timeout;

        timeout.tv_sec  = timeout_in_milliseconds / 1000;
        timeout.tv_usec = (timeout_in_milliseconds % 1000) * 1000;

        SetSocketOption<timeval> (SO_RCVTIMEO, timeout);

        int received_bytes = recv (socket, (char*)buffer, size, 0);

        if (received_bytes < 0)
        {
          if (WSAGetLastError () == WSAEWOULDBLOCK)
            return 0;
          else
            raise_error ("::recv");
        }

        return received_bytes;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::Receive");
        throw;
      }
    }

    size_t Send (const void* buffer, size_t size, size_t timeout_in_milliseconds)
    {
      try
      {
        timeval timeout;

        timeout.tv_sec  = timeout_in_milliseconds / 1000;
        timeout.tv_usec = (timeout_in_milliseconds % 1000) * 1000;

        SetSocketOption<timeval> (SO_SNDTIMEO, timeout);

        int sent_bytes = send (socket, (const char*)buffer, size, 0);

        if (sent_bytes < 0)
        {
          if (WSAGetLastError () == WSAEWOULDBLOCK)
            return 0;
          else
            raise_error ("::send");
        }

        return sent_bytes;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::Send");
        throw;
      }
    }

///Количество байт доступных для чтения без блокировки
    size_t ReceiveAvailable ()
    {
      try
      {
        u_long return_value;

        if (ioctlsocket (socket, FIONREAD, &return_value))
          raise_error ("::ioctlsocket");

        return return_value;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::ReceiveAvailable");
        throw;
      }
    }

///Подсчёт ссылок
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    template <class T>
    T GetSocketOption (int option, int level = SOL_SOCKET)
    {
      T         option_value;
      socklen_t option_length = sizeof (option_value);

      try
      {
        if (getsockopt (socket, level, option, (char*)&option_value, &option_length))
          raise_error ("::getsockopt");
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::GetSocketOption");
        throw;
      }

      return option_value;
    }

    template <class T>
    void SetSocketOption (int option, const T& value, int level = SOL_SOCKET)
    {
      try
      {
        if (setsockopt (socket, level, option, (const char*)&value, sizeof (T)))
          raise_error ("::setsockopt");
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::SetSocketOption");
        throw;
      }
    }

    void FillSockaddrStorage (const SocketAddress& address, sockaddr_storage& address_storage, size_t& address_storage_size)
    {
      switch (address.InetAddress ().Size ())
      {
        case 4:
        {
          address_storage.ss_family = AF_INET;
          address_storage_size      = sizeof (sockaddr_in);

          sockaddr_in* address_desc = (sockaddr_in*)&address_storage;

          address_desc->sin_port = address.Port ();

          address_desc->sin_addr.s_addr = 0;

          address_desc->sin_addr.s_addr |= (int)address.InetAddress () [0];
          address_desc->sin_addr.s_addr |= ((int)address.InetAddress () [1]) << 8;
          address_desc->sin_addr.s_addr |= ((int)address.InetAddress () [2]) << 16;
          address_desc->sin_addr.s_addr |= ((int)address.InetAddress () [3]) << 24;

          break;
        }
        case 16:
        {
          address_storage.ss_family = AF_INET6;
          address_storage_size      = sizeof (sockaddr_in6);

          sockaddr_in6* address_desc = (sockaddr_in6*)&address_storage;

          address_desc->sin6_port = address.Port ();

          for (size_t i = 0; i < 16; i++)
            address_desc->sin6_addr.s6_addr [i] = address.InetAddress () [i];

          break;
        }
        default:
          throw xtl::format_operation_exception ("network::WinSocket::FillSockaddrStorage", "Unsupported internet address size %u", address.InetAddress ().Size ());
      }
    }

    SocketAddress GetSocketAddress (const sockaddr_storage& address_storage, int address_storage_size)
    {
      static const char* METHOD_NAME = "network::WinSocket::GetSocketAddress";

      size_t         address_size;
      unsigned char  address [16];
      unsigned short port;

      switch (address_storage.ss_family)
      {
        case AF_INET:
        {
          if (address_storage_size != sizeof (sockaddr_in))
            throw xtl::format_operation_exception (METHOD_NAME, "Error while calling ::getsockname, address family is AF_INET, but address size not equal to size of sockaddr_in struct");

          address_size = 4;

          sockaddr_in* address_desc = (sockaddr_in*)&address_storage;

          port = address_desc->sin_port;

          memcpy (address, &address_desc->sin_addr, address_size);
          break;
        }
        case AF_INET6:
        {
          if (address_storage_size != sizeof (sockaddr_in6))
            throw xtl::format_operation_exception (METHOD_NAME, "Error while calling ::getsockname, address family is AF_INET, but address size not equal to size of sockaddr_in6 struct");

          address_size = 16;

          sockaddr_in6* address_desc = (sockaddr_in6*)&address_storage;

          port = address_desc->sin6_port;

          memcpy (address, &address_desc->sin6_addr, address_size);
          break;
        }
        default:
          throw xtl::format_operation_exception (METHOD_NAME, "Unsupported socket address family %d", address_storage.ss_family);
      }

      return SocketAddress (InetAddress (address_size, address), port);
    }

    void SetBlockingMode (bool state)
    {
      try
      {
        if (state)
        {
          WSAEVENT dummy_event = WSACreateEvent ();

          if (dummy_event == WSA_INVALID_EVENT)
            raise_error ("::WSACreateEvent");

          if (WSAEventSelect (socket, dummy_event, 0) == SOCKET_ERROR)
            raise_error ("::WSAEventSelect");
        }

        u_long state_value = state ? 0 : 1;

        if (ioctlsocket (socket, FIONBIO, &state_value) == SOCKET_ERROR)
          raise_error ("::ioctlsocket");
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::WinSocket::SetBlockingMode");
        throw;
      }
    }

  private:
    SOCKET        socket;                   //дескриптор сокета
    SocketAddress local_address;            //локальный адресс сокета
    bool          local_address_getted;     //был ли получен локальный адресс
    SocketAddress remote_address;           //удаленный адресс сокета
    bool          remote_address_getted;    //был ли получен удаленный адресс
    bool          receive_closed;           //закрыт ли прием
    bool          send_closed;              //закрыта ли передача
    bool          connected;                //соединен ли сокет
    bool          bound;                    //привязан ли сокет
};

}

/*
   Создание сокета
*/

SocketImpl* Win32Platform::CreateSocket (SocketDomain socket_domain, SocketProtocol protocol)
{
  init_networking ();

  return new WinSocket (socket_domain, protocol);
}
