#include "shared.h"

using namespace network;

namespace
{

/*
    Константы
*/

const char*  DEFAULT_URL              = "http://localhost/";  //URL по умолчанию
const char*  DEFAULT_CONTENT_TYPE     = "text";               //тип контента по умолчанию
const char*  DEFAULT_CONTENT_ENCODING = "utf-8";              //кодировка контента по умолчанию
const size_t RECEIVE_BLOCK_SIZE       = 16384;                //размер буфера приёма по умолчанию

/*
    Вспомогательные структуры
*/

struct ReceiveBlock: public xtl::reference_counter
{
  char buffer [RECEIVE_BLOCK_SIZE]; //буфер данных
};

typedef xtl::intrusive_ptr<ReceiveBlock> ReceiveBlockPtr;
typedef stl::list<ReceiveBlockPtr>       ReceiveBlockList;

}

/*
    Описание реализации URL соединения
*/

struct UrlConnection::Impl: public xtl::reference_counter, public IUrlStream::IListener
{
  syslib::Mutex             mutex;            //блокировка соединения
  syslib::Condition         recv_condition;   //события приёма данных
  network::Url              url;              //URL ресурса
  stl::auto_ptr<IUrlStream> stream;           //поток URL данных
  stl::string               content_type;     //тип контента
  stl::string               content_encoding; //кодировка
  size_t                    content_length;   //длина контента
  size_t                    total_send_size;  //общий объём переданных данных
  size_t                    total_recv_size;  //общий объём полученных данных
  size_t                    recv_offset;      //смещение для первого блока полученных данных
  ReceiveBlockList          recv_list;        //список принятых блоков
  bool                      recv_finish;      //приём данных окончен

///Конструкторы
  Impl ()
    : url (DEFAULT_URL)
    , content_type (DEFAULT_CONTENT_TYPE)
    , content_encoding (DEFAULT_CONTENT_ENCODING)
    , content_length (0)
    , total_send_size (0)
    , total_recv_size (0)
    , recv_offset (0)
    , recv_finish (true)
  {
  }
  
  Impl (const network::Url& in_url, const char* params)
    : url (in_url)
    , content_length (0)
    , total_send_size (0)
    , total_recv_size (0)
    , recv_offset (0)
    , recv_finish (false)
  {
    stream = stl::auto_ptr<IUrlStream> (UrlStreamManagerSingleton::Instance ()->CreateStream (url.ToString (), params, *this));
    
    if (!&*stream)
      throw xtl::format_operation_exception ("", "Internal error: Null stream returned by stream manager");
    
    content_type     = stream->GetContentType ();
    content_encoding = stream->GetContentEncoding ();
    content_length   = stream->GetContentLength ();
  }
  
///Деструктор
  ~Impl ()
  {
    mutex.Lock (); //без вызова Unlock, поскольку Mutex будет удалён
  }
  
///Обработка события получения данных
  void WriteReceivedData (const void* buffer, size_t size)
  {
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("network::UrlConnection::Impl::WriteReceivedData", "buffer");

    syslib::Lock lock (mutex);

    const char* src = reinterpret_cast<const char*> (buffer);
      
    while (size)
    {
      size_t available_size       = total_recv_size - recv_offset,
             block_offset         = available_size % RECEIVE_BLOCK_SIZE,
             available_block_size = RECEIVE_BLOCK_SIZE - block_offset,
             write_size           = size < available_block_size ? size : available_block_size;
             
      ReceiveBlockPtr block = block_offset == 0 ? ReceiveBlockPtr (new ReceiveBlock, false) : recv_list.back ();
      
      memcpy (block->buffer + block_offset, src, write_size);
            
      if (block != recv_list.back ())
        recv_list.push_back (block);
        
      size            -= write_size;
      src             += write_size;
      total_recv_size += write_size;
    }
    
    recv_condition.NotifyAll ();
  }
  
///Конец приёма данных
  void FinishReceiveData ()
  {
    syslib::Lock lock (mutex);
    
    recv_finish = true;
  }
  
///Приём данных
  size_t Receive (void* buffer, size_t size)
  {
    syslib::Lock lock (mutex);    
    
    if (!&*stream)
      throw xtl::format_operation_exception ("", "Can't receive data from closed URL stream");

    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");

    if (!size)
      return 0;
      
    size_t result = 0;
    
    char* dst = reinterpret_cast<char*> (buffer);

    while (size)
    {
        //ожидание данных
        
      size_t available;
      
      while ((available = total_recv_size - recv_offset) == 0)
      {
        if (recv_finish)
          return result;
          
        recv_condition.Wait (mutex);
      }
      
        //получение блока
        
      if (recv_list.empty ())
        throw xtl::format_operation_exception ("", "Internal error: empty receive list");
      
      ReceiveBlockPtr block = recv_list.front ();
      
        //расчёт смещений
        
      size_t block_offset         = recv_offset % RECEIVE_BLOCK_SIZE,
             block_available_size = RECEIVE_BLOCK_SIZE - recv_offset,
             read_size            = size < block_available_size ? size : block_available_size;
             
      if (read_size > available)
        read_size = available;
      
        //чтение данных
        
      memcpy (dst, block->buffer + block_offset, read_size);
      
        //обновление списка
        
      if (read_size == block_available_size)
        recv_list.pop_front ();
      
        //обновление указателей
        
      result      += read_size;
      size        -= read_size;
      dst         += read_size;
      recv_offset += read_size;
    }
    
    return result;
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

UrlConnection::UrlConnection ()
  : impl (new Impl)
{
}

UrlConnection::UrlConnection (const network::Url& url, const char* params)
{
  try
  {
    impl = new Impl (url, params);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlConnection::UrlConnection(const network::Url&,const char*)");
    throw;
  }
}

UrlConnection::UrlConnection (const UrlConnection& connection)
  : impl (connection.impl)
{
  syslib::Lock lock (impl->mutex);

  addref (impl);
}

UrlConnection::~UrlConnection ()
{
  release (impl);
}

UrlConnection& UrlConnection::operator = (const UrlConnection& connection)
{
  UrlConnection (connection).Swap (*this);

  return *this;
}

/*
    Идентификатор ресурса
*/

const network::Url& UrlConnection::Url () const
{
  syslib::Lock lock (impl->mutex);

  return impl->url;
}

/*
    Закрытие соединения
*/

void UrlConnection::Close ()
{
  syslib::Lock lock (impl->mutex);
  
  UrlConnection ().Swap (*this);
}

bool UrlConnection::IsClosed () const
{
  syslib::Lock lock (impl->mutex);

  return &*impl->stream == 0;
}

/*
    Параметры содержимого
*/

size_t UrlConnection::ContentLength () const
{
  return impl->content_length;
}

const char* UrlConnection::ContentEncoding () const
{
  return impl->content_encoding.c_str ();
}

const char* UrlConnection::ContentType () const
{
  return impl->content_type.c_str ();
}

/*
    Чтение / запись данных
*/

size_t UrlConnection::Receive (void* buffer, size_t size)
{
  try
  {
    return impl->Receive (buffer, size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlConnection::Receive");
    throw;
  }  
}

size_t UrlConnection::Send (const void* buffer, size_t size)
{
  try
  {
    syslib::Lock lock (impl->mutex);    
    
    if (!&*impl->stream)
      throw xtl::format_operation_exception ("", "Can't send data to closed URL stream");
    
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");
      
    if (!size)
      return 0;

    return impl->stream->SendData (buffer, size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlConnection::Send");
    throw;
  }
}

/*
    Размеры окна приёма / передачи
*/

//количество байт доступных для чтения без блокировки
size_t UrlConnection::ReceiveAvailable () const
{
  syslib::Lock lock (impl->mutex);

  return impl->total_recv_size - impl->recv_offset;
}

//общее число полученных байт
size_t UrlConnection::ReceivedDataSize () const
{
  syslib::Lock lock (impl->mutex);

  return impl->total_recv_size;
}

//общее число переданных байт
size_t UrlConnection::SendDataSize () const
{
  syslib::Lock lock (impl->mutex);

  return impl->total_send_size;
}

/*
    Обмен
*/

void UrlConnection::Swap (UrlConnection& connection)
{
  syslib::Lock lock1 (impl->mutex);
  syslib::Lock lock2 (connection.impl->mutex);

  stl::swap (impl, connection.impl);
}

namespace network
{

void swap (UrlConnection& connection1, UrlConnection& connection2)
{
  connection1.Swap (connection2);
}

}
