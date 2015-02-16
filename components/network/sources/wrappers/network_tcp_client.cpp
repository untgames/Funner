#include "shared.h"

using namespace network;

//TODO: настроить работу таймаутов
//TODO: сделать select-based обработку
//TODO: использовать threadpool

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' is used in base member initializer list
#endif

namespace
{

/*
    Константы
*/

const unsigned int DEFAULT_ASYNC_RECEIVE_BUFFER_RECEIVE_SIZE = 16384;
const unsigned int ASYNC_SENDING_QUEUE_MAX_SIZE              = 16;
const unsigned int ASYNC_TIMEOUT_IN_MILLISECONDS             = 1000;

/*
    Типы
*/

typedef xtl::uninitialized_storage<char> Buffer;

/*
    Реализация асинхронного доступа
*/

///Блок с данными
struct Block
{
  typedef xtl::com_ptr<Block> Pointer;

  xtl::reference_counter ref_count; //количество ссылок на буфер
  unsigned int           size;      //размер буфера
  char                   data [1];  //данные

///Конструктор
  Block (unsigned int in_size, const void* in_data)
    : size (in_size)
  {
    memcpy (data, in_data, size);
  }

///Создание буфера
  static Pointer Create (unsigned int size, const void* data)
  {
    Block* block = new (stl::allocate<char> (sizeof (Block) - sizeof (char) + size)) Block (size, data);

    return Pointer (block, false);
  }

///Подсчет ссылок
  void AddRef () { ref_count.increment (); }

  void Release ()
  {
    if (ref_count.decrement ())
    {
      const unsigned int buf_size = size;

      stl::destroy (this);
      stl::deallocate ((void*)this, buf_size);
    }
  }
};

typedef Block::Pointer BlockPtr;

///Очередь с возможностью доступа из нескольких нитей
class SharedQueue
{
  public:
///Конструктор
    SharedQueue (unsigned int in_max_size)
      : max_size (in_max_size)
    {      
    }
      
///Деструктор
    ~SharedQueue ()
    {
      mutex.Lock ();
    }

///Помещение буфера в очередь
    bool Push (const BlockPtr& block, unsigned int timeout_in_milliseconds)
    {
      syslib::Lock lock (mutex);

      size_t start_time = common::milliseconds ();

      while (blocks.size () >= max_size)
      {
        size_t delay = start_time + timeout_in_milliseconds - common::milliseconds ();

        if (delay > timeout_in_milliseconds || !delay)
          return false;

        full_condition.TryWait (mutex, delay);        
      }

      blocks.push_back (block);

      empty_condition.NotifyOne ();

      return true;
    }

///Извлечение буфера из очереди
    BlockPtr Pop (unsigned int timeout_in_milliseconds)
    {
      syslib::Lock lock (mutex);

      size_t start_time = common::milliseconds ();

      while (blocks.empty ())      
      {
        size_t delay = start_time + timeout_in_milliseconds - common::milliseconds ();

        if (delay > timeout_in_milliseconds || !delay)
          return BlockPtr ();

        empty_condition.TryWait (mutex, delay);
      }

      BlockPtr result = blocks.front ();

      blocks.pop_front ();

      full_condition.NotifyOne ();

      return result;
    }

  private:
    typedef stl::deque<BlockPtr> BlockQueue;

  private:
    syslib::Mutex     mutex;           //блокировка доступа к данным очереди
    syslib::Condition full_condition;  //событие изменения состояния очереди
    syslib::Condition empty_condition; //событие изменения состояния очереди
    BlockQueue        blocks;          //блоки
    unsigned int      max_size;        //максимальный размер очереди
};

///Асинхронная обработка данных
class AsyncProcessor
{
  public:
///Конструктор
    AsyncProcessor (const char* name)
      : log (name)
      , stop_requested (false)
      , thread (name, xtl::bind (&AsyncProcessor::Run, this))
    {
    }

///Деструктор
    virtual ~AsyncProcessor ()
    {
      try
      {
        Stop ();
      }
      catch (...)
      {
      }
    }

///Остановка обработки
    void Stop ()
    {
      stop_requested = true;

      thread.Join ();
    }

    void StopRequest () { stop_requested = true; }

///Запрошена ли остановка
    bool IsStopRequested () const { return stop_requested; }

  private:
///Обработка очереди
    int Run ()
    {
      while (!stop_requested)
      {
        try
        {
          DoStep ();
        }
        catch (std::exception& e)
        {
          log.Printf ("%s\n    at network::AsyncProcessor::Run", e.what ());
        }
        catch (...)
        {
          log.Printf ("unknown exception\n    at network::AsyncProcessor::Run");
        }
      }

      return 0;
    }

///Итерация обработки очереди
    virtual void DoStep () = 0;

  private:    
    common::Log    log;            //поток отладочного протоколирования
    volatile bool  stop_requested; //запрошена остановка
    syslib::Thread thread;         //нить отсылки данных
};

///Асинхронная отсылка данных
class AsyncSender: public AsyncProcessor
{
  public:
///Конструктор
    AsyncSender (Socket& in_socket)
      : AsyncProcessor ("network.AsyncSender")
      , socket (in_socket)
      , queue (ASYNC_SENDING_QUEUE_MAX_SIZE)
    {
    }

///Деструктор
    ~AsyncSender ()
    {
      try
      {
        Stop ();
      }
      catch (...)
      {
      }
    }

///Запрос на отсылку данных
    void SendRequest (const void* buffer, unsigned int size)
    {
      try
      { 
        while (!queue.Push (Block::Create (size, buffer), ASYNC_TIMEOUT_IN_MILLISECONDS) && !IsStopRequested ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::AsyncSender");
        throw;
      }
    }

  private:
///Итерация обработки очереди
    void DoStep ()
    {
      BlockPtr block = queue.Pop (ASYNC_TIMEOUT_IN_MILLISECONDS);  

      if (!block)
        return;

      unsigned int size = block->size;
      char*  data = block->data;

      while (size)
      {
        if (IsStopRequested ())
          break;

        unsigned int send_size = socket.Send (data, size, ASYNC_TIMEOUT_IN_MILLISECONDS);

        if (!send_size)
        {
          if (socket.IsSendClosed ())
            StopRequest ();

          return;
        }

        size -= send_size;
      }
    }

  private:    
    Socket&     socket; //сокет
    SharedQueue queue;  //очередь отсылки
};

///Асинхронный прием данных
class AsyncReceiver: public AsyncProcessor, public common::Lockable
{
  public:
///Конструктор
    AsyncReceiver (Socket& in_socket, const TcpClient::AsyncReceivingHandler& in_handler)
      : AsyncProcessor ("network.AsyncReceiver")
      , socket (in_socket)
      , handler (in_handler)
    {
      buffer.resize (DEFAULT_ASYNC_RECEIVE_BUFFER_RECEIVE_SIZE);
    }

///Деструктор
    ~AsyncReceiver ()
    {
      try
      {
        Stop ();

        Lock ();
      }
      catch (...)
      {
      }
    }

  private:
///Итерация обработки очереди
    void DoStep ()
    {
      unsigned int recv_size = socket.Receive (buffer.data (), (unsigned int)buffer.size (), ASYNC_TIMEOUT_IN_MILLISECONDS);

      if (!recv_size)
      {
        if (socket.IsReceiveClosed ())
          StopRequest ();

        return;
      }

      common::Lock lock (*this);

      handler (buffer.data (), recv_size);
    }

  private:    
    Socket&                          socket;  //сокет
    Buffer                           buffer;  //буфер приемки
    TcpClient::AsyncReceivingHandler handler; //обработчик получения данных
};

}

/*
    Описание реализации клиента
*/

typedef xtl::signal<void (const void*, unsigned int)> AsyncDataReceivingSignal;

struct TcpClient::Impl: public xtl::reference_counter, public common::Lockable
{
  Socket                       socket;                    //сокет
  Buffer                       recv_buffer;               //буфер с принятыми данными
  char*                        recv_start;                //начало принятых данных
  char*                        recv_finish;               //конец принятых данных
  stl::auto_ptr<AsyncReceiver> async_receiver;            //асинхронный приём данных
  stl::auto_ptr<AsyncSender>   async_sender;              //асинхронная отсылка данных
  AsyncDataReceivingSignal     async_receive_data_signal; //сигнал оповещения о получении данных
  
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
    stl::auto_ptr<AsyncReceiver> tmp_async_receiver;
    stl::auto_ptr<AsyncSender>   tmp_async_sender;
  
    {
      common::Lock lock (*this);

      tmp_async_sender   = async_sender;
      tmp_async_receiver = async_receiver;
    }

    tmp_async_sender.reset ();
    tmp_async_receiver.reset ();

    {
      common::Lock lock (*this);

      socket.Close ();

      recv_buffer.resize (0);

      recv_start = recv_finish = recv_buffer.data ();
    }
  }
  
///Принятие данных
  unsigned int Receive (void* buffer, unsigned int size, unsigned int timeout_in_milliseconds)
  {
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");
      
    if (!size)
      return 0;
      
    char*        pos       = (char*)buffer;
    unsigned int recv_size = 0;

    {
      common::Lock lock (*this);

      unsigned int received = (unsigned int)(recv_finish - recv_start);
      
      if (received)
      {
        unsigned int copy_size = stl::min (received, size);
        
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
  
  bool ReceiveExactly (void* buffer, unsigned int size, unsigned int timeout_in_milliseconds)
  {
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");
      
    if (!size)
      return true;

    char* pos = (char*)buffer;
    
    size_t       start_time = common::milliseconds ();
    unsigned int recv_size  = 0;

    try
    {
      while (size)
      {
        size_t time_spent = common::milliseconds () - start_time;

        if (time_spent > timeout_in_milliseconds)
          throw xtl::format_operation_exception ("", "Timeout in ReceiveExactly");

        unsigned int cur_recv_size = 0;

        try
        {
          cur_recv_size = Receive (pos, size, (unsigned int)(timeout_in_milliseconds - time_spent));
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

///Асинхронный обработчик получения данных
  void OnAsyncDataReceived (const void* data, unsigned int size)
  {
    common::Lock lock (*this);

    async_receive_data_signal (data, size);
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

TcpClient::TcpClient ()
  : impl (new Impl)
{
}

TcpClient::TcpClient (const SocketAddress& address, unsigned int timeout_in_milliseconds, bool tcp_no_delay)
  : impl (new Impl)
{
  try
  {
    Connect (address, timeout_in_milliseconds, tcp_no_delay);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::TcpClient(const SocketAddress&,unsigned int,bool)");
    throw;
  }
}

TcpClient::TcpClient (const InetAddress& address, unsigned short port, unsigned int timeout_in_milliseconds, bool tcp_no_delay)
  : impl (new Impl)
{
  try
  {
    Connect (SocketAddress (address, port), timeout_in_milliseconds, tcp_no_delay);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::TcpClient(const InetAddress&,unsigned short,unsigned int,bool)");
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

void TcpClient::Connect (const SocketAddress& address, unsigned int timeout_in_milliseconds, bool tcp_no_delay)
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
    e.touch ("network::TcpClient::Connect(const InetAddress&,unsigned short,unsigned int,bool)");
    throw;
  }
}

void TcpClient::Connect (const InetAddress& address, unsigned short port, unsigned int timeout_in_milliseconds, bool tcp_no_delay)
{
  try
  {
    Connect (SocketAddress (address, port), timeout_in_milliseconds, tcp_no_delay);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Connect(const InetAddress&,unsigned short,unsigned int,bool)");
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

void TcpClient::Send (const void* buffer, unsigned int size)
{
  try
  {
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");

    {
      common::Lock lock (*impl);

      if (impl->async_sender)
      {
        impl->async_sender->SendRequest (buffer, size);
        return;
      }
    }

    const char* data = (const char*)buffer;
      
    while (size)
    {
      unsigned int send_size = impl->socket.Send (data, size);

      size -= send_size;
      data += send_size;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Send(const void* unsigned int)");
    throw;
  }
}

void TcpClient::Send (const char* string)
{
  try
  {
    if (!string)
      throw xtl::make_null_argument_exception ("", "string");

    Send (string, (unsigned int)xtl::xstrlen (string));
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

unsigned int TcpClient::Receive (void* buffer, unsigned int size, unsigned int timeout_in_milliseconds)
{
  try
  {
    {
      common::Lock lock (*impl);
    
      if (impl->async_receiver.get ())
        return 0;
    }

    return impl->Receive (buffer, size, timeout_in_milliseconds);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Receive(void*,unsigned int,unsigned int)");
    throw;
  }
}

bool TcpClient::ReceiveExactly (void* buffer, unsigned int size, unsigned int timeout_in_milliseconds)
{
  try
  {
    {
      common::Lock lock (*impl);
    
      if (impl->async_receiver.get ())
        return false;
    }

    return impl->ReceiveExactly (buffer, size, timeout_in_milliseconds);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::ReceiveExactly(void*,unsigned int,unsigned int)");
    throw;
  }
}

/*
    Количество байт доступных для чтения без блокировки
*/

unsigned int TcpClient::ReceiveAvailable () const
{
  try
  {
    common::Lock lock (*impl);

    return (unsigned int)(impl->recv_finish - impl->recv_start + impl->socket.ReceiveAvailable ());
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

void TcpClient::SetReceiveBufferSize (unsigned int size)
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

void TcpClient::SetSendBufferSize (unsigned int size)
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

unsigned int TcpClient::ReceiveBufferSize () const
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

unsigned int TcpClient::SendBufferSize () const
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
    Асинхронный режим
*/

bool TcpClient::IsAsyncSendingEnabled () const
{
  common::Lock lock (*impl);

  return impl->async_sender;
}

bool TcpClient::IsAsyncReceivingEnabled () const
{
  common::Lock lock (*impl);

  return impl->async_receiver;
}

void TcpClient::SwitchToAsyncReceiving () const
{
  common::Lock lock (*impl);

  if (impl->async_receiver)
    return;

  unsigned int available = (unsigned int)(impl->recv_finish - impl->recv_start);

  if (available)
  {
    impl->async_receive_data_signal (impl->recv_start, available);

    impl->recv_start = impl->recv_finish = impl->recv_buffer.data ();
  }

  impl->async_receiver.reset (new AsyncReceiver (impl->socket, xtl::bind (&Impl::OnAsyncDataReceived, &*impl, _1, _2)));
}

void TcpClient::SwitchToAsyncSending () const
{
  common::Lock lock (*impl);

  if (impl->async_sender)
    return;

  impl->async_sender.reset (new AsyncSender (impl->socket));
}

xtl::connection TcpClient::RegisterAsyncReceivingEventHandler (const AsyncReceivingHandler& handler)
{
  common::Lock lock (*impl);

  return impl->async_receive_data_signal.connect (handler);
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
