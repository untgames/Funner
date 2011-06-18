#include "shared.h"

using namespace network;

/*
    Описание реализации клиента
*/

typedef xtl::uninitialized_storage<char> Buffer;

struct TcpClient::Impl: public xtl::reference_counter, public common::Lockable
{
  Socket socket;      //сокет
  Buffer recv_buffer; //буфер с принятыми данными
  char*  recv_start;  //начало принятых данных
  char*  recv_finish; //конец принятых данных
  
///Конструктор
  Impl ()
  {  
    recv_start = recv_finish = recv_buffer.data ();
  }
  
///Деструктор
  ~Impl ()
  {
    try
    {
      Close ();
    }
    catch (...)
    {      
    }
  }
  
///Закрытие канала
  void Close ()
  {
    socket.Close ();
    recv_buffer.resize (0);
    
    recv_start = recv_finish = recv_buffer.data ();
  }
  
///Принятие данных
  size_t Receive (void* buffer, size_t size, size_t timeout_in_milliseconds)
  {
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");
      
    if (!size)
      return 0;
      
    char*  pos       = (char*)buffer;
    size_t recv_size = 0;

    {
      common::Lock lock (*this);

      size_t received = recv_finish - recv_start;
      
      if (received)
      {
        size_t copy_size = stl::min (received, size);
        
        memcpy (pos, recv_start, copy_size);
        
        pos        += copy_size;
        recv_start += copy_size;
        recv_size  += copy_size;        
        size       -= copy_size;
      }
    }

    if (size)
      recv_size += socket.Receive (pos, size, timeout_in_milliseconds);     

    return recv_size;
  }
  
  bool ReceiveExactly (void* buffer, size_t size, size_t timeout_in_milliseconds)
  {
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");
      
    if (!size)
      return true;

    char* pos = (char*)buffer;
    
    size_t start_time = common::milliseconds (),
           recv_size  = 0;

    try
    {
      while (size)
      {
        size_t time_spent = common::milliseconds () - start_time;

        if (time_spent > timeout_in_milliseconds)
          throw xtl::format_operation_exception ("", "Timeout in ReceiveExactly");

        size_t cur_recv_size = 0;

        try
        {
          cur_recv_size = Receive (pos, size, timeout_in_milliseconds - time_spent);
        }
        catch (...)
        {
        }
        
        if (!cur_recv_size && socket.IsReceiveClosed () && !socket.ReceiveAvailable ())
          throw xtl::format_operation_exception ("", "Connection closed by counterparty");
       
        recv_size += cur_recv_size;
        pos       += cur_recv_size;
        size      -= cur_recv_size;        
      }
      
      return true;
    }
    catch (...)
    {
      common::Lock lock (*this);

      if (recv_size && recv_start != recv_finish)
        throw xtl::format_operation_exception ("network::TcpClient::Impl::ReceiveExactly", "Internal error: recv_size && recv_start != recv_finish");

      if (recv_size)
      {
        if (recv_size > recv_buffer.size ())
          recv_buffer.resize (recv_size, false);
        
        recv_start  = recv_buffer.data ();
        recv_finish = recv_start + recv_size;
        
        memcpy (recv_start, pos - recv_size, recv_size);
      }

      return false;
    }
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

TcpClient::TcpClient ()
  : impl (new Impl)
{
}

TcpClient::TcpClient (const SocketAddress& address, size_t timeout_in_milliseconds, bool tcp_no_delay)
  : impl (new Impl)
{
  try
  {
    Connect (address, timeout_in_milliseconds, tcp_no_delay);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::TcpClient(const SocketAddress&,size_t,bool)");
    throw;
  }
}

TcpClient::TcpClient (const InetAddress& address, unsigned short port, size_t timeout_in_milliseconds, bool tcp_no_delay)
  : impl (new Impl)
{
  try
  {
    Connect (SocketAddress (address, port), timeout_in_milliseconds, tcp_no_delay);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::TcpClient(const InetAddress&,unsigned short,size_t,bool)");
    throw;
  }
}

TcpClient::TcpClient (const TcpClient& client)
  : impl (client.impl)
{
  addref (impl);
}

TcpClient::~TcpClient ()
{
  release (impl);
}

TcpClient& TcpClient::operator = (const TcpClient& client)
{
  TcpClient (client).Swap (*this);
  
  return *this;
}

/*
    Соединение
*/

void TcpClient::Connect (const SocketAddress& address, size_t timeout_in_milliseconds, bool tcp_no_delay)
{
  try
  {
    if (!IsClosed ())
      Close ();

    common::Lock lock (*impl);
    
    Socket (SocketDomain_IpV4, SocketProtocol_Tcp).Swap (impl->socket);

    impl->socket.Connect (address, timeout_in_milliseconds);

    try
    {
      SetTcpNoDelay (tcp_no_delay);
    }
    catch (...)
    {
      Close ();
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Connect(const InetAddress&,unsigned short,size_t,bool)");
    throw;
  }
}

void TcpClient::Connect (const InetAddress& address, unsigned short port, size_t timeout_in_milliseconds, bool tcp_no_delay)
{
  try
  {
    Connect (SocketAddress (address, port), timeout_in_milliseconds, tcp_no_delay);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Connect(const InetAddress&,unsigned short,size_t,bool)");
    throw;
  }
}

void TcpClient::Close ()
{
  try
  {
    impl->Close ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Close");
    throw;
  }
}

/*
    Передача данных
*/

void TcpClient::Send (const void* buffer, size_t size)
{
  try
  {
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");
      
    while (size)
    {
      size_t send_size = impl->socket.Send (buffer, size);

      size -= send_size;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Send(const void* size_t)");
    throw;
  }
}

void TcpClient::Send (const char* string)
{
  try
  {
    if (!string)
      throw xtl::make_null_argument_exception ("", "string");

    Send (string, strlen (string));
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Send(const char*)");
    throw;
  }
}

/*
    Приём данных
*/

size_t TcpClient::Receive (void* buffer, size_t size, size_t timeout_in_milliseconds)
{
  try
  {
    return impl->Receive (buffer, size, timeout_in_milliseconds);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Receive(void*,size_t,size_t)");
    throw;
  }
}

bool TcpClient::ReceiveExactly (void* buffer, size_t size, size_t timeout_in_milliseconds)
{
  try
  {
    return impl->ReceiveExactly (buffer, size, timeout_in_milliseconds);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::ReceiveExactly(void*,size_t,size_t)");
    throw;
  }
}

/*
    Количество байт доступных для чтения без блокировки
*/

size_t TcpClient::ReceiveAvailable () const
{
  try
  {
    common::Lock lock (*impl);

    return impl->recv_finish - impl->recv_start + impl->socket.ReceiveAvailable ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::ReceiveAvailable");
    throw;
  }
}

/*
    Параметры сокета
*/

void TcpClient::SetReceiveBufferSize (size_t size)
{
  try
  {
    impl->socket.SetReceiveBufferSize (size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::SetReceiveBufferSize");
    throw;
  }
}

void TcpClient::SetSendBufferSize (size_t size)
{
  try
  {
    impl->socket.SetSendBufferSize (size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::SetSendBufferSize");
    throw;
  }
}

size_t TcpClient::ReceiveBufferSize () const
{
  try
  {
    return impl->socket.ReceiveBufferSize ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::ReceiveBufferSize");
    throw;
  }
}

size_t TcpClient::SendBufferSize () const
{
  try
  {
    return impl->socket.SendBufferSize ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::SendBufferSize");
    throw;
  }
}

/*
    Флаги сокета
*/

bool TcpClient::IsClosed () const
{
  try
  {
    return impl->socket.IsClosed ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::IsClosed");
    throw;
  }
}

bool TcpClient::IsConnected () const
{
  try
  {
    return impl->socket.IsConnected ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::IsConnected");
    throw;
  }
}

bool TcpClient::IsTcpNoDelay () const
{
  try
  {
    return impl->socket.IsTcpNoDelay ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::IsTcpNoDelay");
    throw;
  }
}

void TcpClient::SetTcpNoDelay (bool state)
{
  try
  {
    impl->socket.SetTcpNoDelay (state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::SetTcpNoDelay");
    throw;
  }
}

/*
    Обмен
*/

void TcpClient::Swap (TcpClient& client)
{
  stl::swap (impl, client.impl);
}

namespace network
{

void swap (TcpClient& client1, TcpClient& client2)
{
  client1.Swap (client2);
}

}
