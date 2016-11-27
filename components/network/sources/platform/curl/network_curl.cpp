#include <curl/curl.h>

#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <syslib/condition.h>
#include <syslib/mutex.h>
#include <syslib/thread.h>

#include <network/url_connection.h>

//TODO: custom управление памятью

using namespace network;

namespace components
{

namespace curl_stream_manager
{

/*
    Константы
*/

const char* LOG_NAME = "network.curl"; //название потока протоколирования

const long MIN_DOWNLOAD_SPEED          = 50;  //Минимальная скорость закачки в байтах в секунду, при падении до которой закачка будет оборвана
const long MIN_DOWNLOAD_SPEED_DURATION = 45;   //Длительность закачки с минимальной скоростью, после которой будет выдана ошибка

/*
    Библиотека CURL
*/

void check_code (CURLcode code, const char* method)
{
  if (code)
    throw xtl::format_operation_exception (method, "Curl error %d. %s", code, curl_easy_strerror (code));
}

void check_mcode (CURLMcode code, const char* method)
{
  if (code)
    throw xtl::format_operation_exception (method, "Curl error %d. %s", code, curl_multi_strerror (code));
}

void check_shcode (CURLSHcode code, const char* method)
{
  if (code)
    throw xtl::format_operation_exception (method, "Curl error %d. %s", code, curl_share_strerror (code));
}

class CurlLibrary
{
  public:
///Конструктор
    CurlLibrary ()
    {
      check_code (curl_global_init (CURL_GLOBAL_ALL), "::curl_global_init");
    }

///Деструктор
    ~CurlLibrary ()
    {
      curl_global_cleanup ();
    }
};

typedef common::Singleton<CurlLibrary> CurlLibrarySingleton;

/*
    URL поток
*/

class CurlStream: public IUrlStream
{
  public:
///Конструктор
    CurlStream (const char* in_url, const char* in_params, IListener& in_listener)
      : log (LOG_NAME)
      , listener (in_listener)
      , stream (0)
      , url (in_url)
      , params (in_params)
      , content_type ("text/plain")
      , content_encoding ("UTF-8")
      , status ("Waiting for headers...")
      , content_length (0)
      , headers_received (false)
      , cancel_operation (false)
    {
      try
      {
        thread = stl::auto_ptr<syslib::Thread> (new syslib::Thread (xtl::bind (&CurlStream::ThreadRoutine, this)));
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::CurlStream::CurlStream");
        throw;
      }
    }

///Деструктор
    ~CurlStream ()
    {
      cancel_operation = true;

      if (thread.get ())
        thread->Join ();

      mutex.Lock ();

      if (stream)
        curl_easy_cleanup (stream);
    }
            
///Параметры потока
    size_t GetContentLength ()
    {
      syslib::Lock lock (mutex);
      
      WaitHeaders ();

      return content_length;
    }
    
    const char* GetContentEncoding ()
    {
      syslib::Lock lock (mutex);      
      
      WaitHeaders ();

      return content_encoding.c_str ();
    }
    
    const char* GetContentType ()
    {
      syslib::Lock lock (mutex);
      
      WaitHeaders ();

      return content_type.c_str ();
    }

    const char* GetStatus ()
    {
      syslib::Lock lock (mutex);

      WaitHeaders ();

      return status.c_str ();
    }        
    
  private:
///Ожидание заголовков
    void WaitHeaders ()
    {
      while (!headers_received)
        headers_condition.Wait (mutex);
    }

    void HeadersReceivedNotify ()
    {
      if (headers_received)
        return;

      headers_received = true;

      headers_condition.NotifyAll ();
    }

///Обработчик нити
    int ThreadRoutine ()
    {
      try
      {
        CurlLibrarySingleton::Instance ();
        
        common::PropertyMap properties = common::parse_init_string (params.c_str ());
        
        stream = curl_easy_init ();
        
        if (!stream)
          throw xtl::format_operation_exception ("", "Can't initialize CURL library");
          
        try
        {
            //общая часть конфигурации соединения
            
          log.Printf ("URL request '%s'", url.c_str ());
          
          check_code (curl_easy_setopt (stream, CURLOPT_URL, url.c_str ()), "::curl_easy_setopt(CURLOPT_URL)");
          check_code (curl_easy_setopt (stream, CURLOPT_WRITEFUNCTION, &WriteDataCallback), "::curl_easy_setopt(CURLOPT_WRITEFUNCTION)");
          check_code (curl_easy_setopt (stream, CURLOPT_WRITEDATA, this), "::curl_easy_setopt(CURLOPT_WRITEDATA)");
          check_code (curl_easy_setopt (stream, CURLOPT_READFUNCTION, &ReadDataCallback), "::curl_easy_setopt(CURLOPT_READFUNCTION)");
          check_code (curl_easy_setopt (stream, CURLOPT_READDATA, this), "::curl_easy_setopt(CURLOPT_READDATA)");
          check_code (curl_easy_setopt (stream, CURLOPT_HEADERFUNCTION, &WriteHeaderCallback), "::curl_easy_setopt(CURLOPT_HEADERFUNCTION)");
          check_code (curl_easy_setopt (stream, CURLOPT_WRITEHEADER, this), "::curl_easy_setopt(CURLOPT_WRITEHEADER)");
          check_code (curl_easy_setopt (stream, CURLOPT_DEBUGFUNCTION, &DebugCallback), "::curl_easy_setopt(CURLOPT_DEBUGFUNCTION)");
          check_code (curl_easy_setopt (stream, CURLOPT_DEBUGDATA, this), "::curl_easy_setopt(CURLOPT_DEBUGDATA)");
          check_code (curl_easy_setopt (stream, CURLOPT_VERBOSE, 1L), "::curl_easy_setopt(CURLOPT_VERBOSE)");
          check_code (curl_easy_setopt (stream, CURLOPT_SSL_VERIFYPEER, false), "::curl_easy_setopt(CURLOPT_SSL_VERIFYPEER)");
          check_code (curl_easy_setopt (stream, CURLOPT_LOW_SPEED_LIMIT, MIN_DOWNLOAD_SPEED), "::curl_easy_setopt(CURLOPT_LOW_SPEED_LIMIT)");
          check_code (curl_easy_setopt (stream, CURLOPT_LOW_SPEED_TIME, MIN_DOWNLOAD_SPEED_DURATION), "::curl_easy_setopt(CURLOPT_LOW_SPEED_TIME)");
          
            //конфигурация соединения, зависящая от параметров
            
          curl_slist* http_header = 0;
          
          http_header = curl_slist_append (http_header, "Expect:");
          
          if (!http_header)
            throw xtl::format_operation_exception ("", "::curl_slist_append failed");                          
            
          if (properties.IsPresent ("method"))
          {
            stl::string method = properties.GetString ("method");
            
            if (method == "post")
            {
              check_code (curl_easy_setopt (stream, CURLOPT_POST, 1L), "::curl_easy_setopt(CURLOPT_HTTPPOST)");
              
              if (properties.IsPresent ("send_size"))
              {
                check_code (curl_easy_setopt (stream, CURLOPT_POSTFIELDSIZE, static_cast<size_t> (properties.GetInteger ("send_size"))), "::curl_easy_setopt(CURLOPT_POSTFIELDSIZE)");                
              }
              else
              {
                http_header = curl_slist_append (http_header, "Transfer-Encoding: chunked");
                
                if (!http_header)
                  throw xtl::format_operation_exception ("", "::curl_slist_append failed");
              }              
            }
          }
          
          if (http_header)
            check_code (curl_easy_setopt (stream, CURLOPT_HTTPHEADER, http_header), "curl_easy_setopt(CURLOPT_HTTPHEADER");          

            //запуск соединения

          check_code (curl_easy_perform (stream), "::curl_perform");

          if (http_header)
            curl_slist_free_all (http_header);

          {
            syslib::Lock lock (mutex);

            HeadersReceivedNotify ();
          }
          
          log.Printf ("URL '%s' received", url.c_str ());
          
          listener.FinishReceiveData ();
          
          return 0;
        }
        catch (...)
        {
          curl_easy_cleanup (stream);
          
          stream = 0;
          
          throw;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::CurlStream::ThreadRoutine");

        SetStatus (e.what ());
        listener.FinishReceiveData ();

        throw;
      }
      catch (std::exception& e)
      {
        SetStatus (e.what ());
        listener.FinishReceiveData ();

        throw;
      }
      catch (...)
      {
        SetStatus ("Unknown exception\n    at network::CurlStream::ThreadRoutine");
        listener.FinishReceiveData ();

        throw;
      }
    }

///Установка статуса
    void SetStatus (const char* in_status)
    {
      syslib::Lock lock (mutex);

      status = in_status;

      HeadersReceivedNotify ();
    }
    
///Обработчик получения заголовка
    static size_t WriteHeaderCallback (void *ptr, size_t size, size_t nmemb, void *userdata)
    {
      if (!userdata)
        return 0;
      
      return reinterpret_cast<CurlStream*> (userdata)->WriteHeader (ptr, size * nmemb);
    }
    
    size_t WriteHeader (void* ptr, size_t size)
    {
      try
      {
        syslib::Lock lock (mutex);

        stl::string str = GetDebugMessage (reinterpret_cast<char*> (ptr), size);
        
        if (common::wcimatch (str.c_str (), "Content-Type:*"))
        {
          common::StringArray tokens = common::parse (str.c_str (), "Content-Type: *([^;]*); *charset=(.*)");
          
          if (tokens.Size () != 3)
          {
            log.Printf ("Bad content-type for URL='%s' (header='%s')", url.c_str (), str.c_str ());
            return size;
          }
            
          content_encoding = tokens [2];
          content_type     = tokens [1];
        }
        else if (common::wcimatch (str.c_str (), "Content-Length:*"))
        {
          common::StringArray tokens = common::parse (str.c_str (), "Content-Length: *(.*)");
          
          if (tokens.Size () != 2)
          {
            log.Printf ("Bad content-length for URL='%s' (header='%s')", url.c_str (), str.c_str ());
            return size;
          }
            
          content_length = atoi (tokens [1]);
        }
        else if (common::wcimatch (str.c_str (), "HTTP/*"))
        {
          common::StringArray tokens = common::parse (str.c_str (), "HTTP/[^ ]* (.*)");
          
          if (tokens.Size () != 2)
          {
            log.Printf ("Bad HTTP header for URL '%s' (header='%s')", url.c_str (), str.c_str ());
            return size;
          }
          
          status = tokens [1];
          
          if (!strncmp (status.c_str (), "200", 3))
            status = "Ok";
        }

        return size;
      }
      catch (std::exception& e)
      {
        log.Printf ("%s\n  at network::CurlStream::WriteHeader", e.what ());
      }
      catch (...)
      {
        log.Printf ("unknown exception\n  at network::CurlStream::WriteHeader");
      }
      
      return 0;     
    }

///Отменена ли операция
    bool IsCanceled ()
    {
      return cancel_operation;
    }

///Обработчик получения данных от CURL
    static size_t WriteDataCallback (void *ptr, size_t size, size_t nmemb, void* userdata)
    {
      if (!userdata)
        return 0;

      CurlStream* stream = reinterpret_cast<CurlStream*> (userdata);

      if (stream->IsCanceled ())
        return 0;

      return stream->WriteReceivedData (ptr, size * nmemb);
    }
    
    size_t WriteReceivedData (void* buffer, size_t size)
    {
      try
      {
        {
          syslib::Lock lock (mutex);

          HeadersReceivedNotify ();
        }
        
        listener.WriteReceivedData (buffer, size);
        
        return size;
      }
      catch (std::exception& e)
      {
        log.Printf ("%s\n  at network::CurlStream::WriteReceivedData", e.what ());
      }
      catch (...)
      {
        log.Printf ("unknown exception\n  at network::CurlStream::WriteReceivedData");
      }
      
      return 0;
    }
    
///Обработчик отсылки данных от CURL
    static size_t ReadDataCallback (void *ptr, size_t size, size_t nmemb, void* userdata)
    {
      if (!userdata)
        return 0;

      CurlStream* stream = reinterpret_cast<CurlStream*> (userdata);

      if (stream->IsCanceled ())
        return CURL_READFUNC_ABORT;

      return stream->ReadSendData (ptr, size * nmemb);
    }
    
    size_t ReadSendData (void* buffer, size_t size)
    {
      try
      {
        size_t result = listener.ReadSendData (buffer, size);;
        
        return result;
      }
      catch (std::exception& e)
      {
        log.Printf ("%s\n  at network::CurlStream::ReadSendData", e.what ());
      }
      catch (...)
      {
        log.Printf ("unknown exception\n  at network::CurlStream::ReadSendData");
      }
      
      return 0;
    }
    
///Отладочное протоколирование
    static int DebugCallback (CURL*, curl_infotype type, char* data, size_t size, void* userdata)
    {
      if (!userdata)
        return 0;

      CurlStream* stream = reinterpret_cast<CurlStream*> (userdata);

      if (stream->IsCanceled ())
        return 0;

      stream->DebugLog (type, data, size);
      
      return 0;
    }
    
    static stl::string GetDebugMessage (const char* data, size_t size)
    {
      stl::string message (data, size);
      
      for (;!message.empty ();)
      {
        switch (message [message.size ()-1])
        {
          case '\n':
          case '\r':
            message.erase (message.size ()-1);
            continue;
          default:
            break;
        }
        
        break;
      }
      
      return message;
    }
    
    void DebugLog (curl_infotype type, char* data, size_t size)
    {
      try
      {
        switch (type)
        {
          case CURLINFO_TEXT:
            log.Printf ("%s", GetDebugMessage (data, size).c_str ());
            break;
          case CURLINFO_HEADER_OUT:
//            log.Printf ("Send header '%s' (URL='%s')", GetDebugMessage (data, size).c_str (), url.c_str ());
            break;
          case CURLINFO_DATA_OUT:
//            log.Printf ("Send data (URL='%s')", url.c_str ());
            break;
          case CURLINFO_SSL_DATA_OUT:
//            log.Printf ("Send SSL data (URL='%s')", url.c_str ());
            break;
          case CURLINFO_HEADER_IN:
            log.Printf ("Receive header '%s' (URL='%s')", GetDebugMessage (data, size).c_str (), url.c_str ());
            break;
          case CURLINFO_DATA_IN:
//            log.Printf ("Receive data (URL='%s')", url.c_str ());
            break;
          case CURLINFO_SSL_DATA_IN:
//            log.Printf ("Receive SSL data (URL='%s')", url.c_str ());
            break;
          default:
            break;
        }
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }
   
  private:
    syslib::Mutex                 mutex;             //блокировка
    common::Log                   log;               //поток протоколирования
    IListener&                    listener;          //слушатель событий потока
    CURL*                         stream;            //поток CURL
    stl::string                   url;               //URL
    stl::string                   params;            //параметры
    stl::auto_ptr<syslib::Thread> thread;            //нить
    stl::string                   content_type;      //тип контента
    stl::string                   content_encoding;  //тип кодировки контента
    stl::string                   status;            //статус
    size_t                        content_length;    //длина контента
    bool                          headers_received;  //заголовки приняты
    syslib::Condition             headers_condition; //событие ожидания заголовков
    volatile bool                 cancel_operation;  //отмена закачки
};


/*
    Компонент запуска CURL
*/

class CurlStreamManagerComponent
{
  public:
    CurlStreamManagerComponent ()    
    {
      UrlStreamManager::RegisterStreamCreator ("http://*", &CurlStreamManagerComponent::CreateStream);
      UrlStreamManager::RegisterStreamCreator ("https://*", &CurlStreamManagerComponent::CreateStream);      
      UrlStreamManager::RegisterStreamCreator ("ftp://*", &CurlStreamManagerComponent::CreateStream);
    }
    
  private:
    static IUrlStream* CreateStream (const char* url, const char* params, IUrlStream::IListener& listener)
    {
      try
      {
        return new CurlStream (url, params, listener);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::CurlStreamManagerComponent::CreateStream");
        throw;
      }
    }
};

extern "C"
{

common::ComponentRegistrator<CurlStreamManagerComponent> CurlStreamManager ("network.url_stream.curl");

}

}

}
