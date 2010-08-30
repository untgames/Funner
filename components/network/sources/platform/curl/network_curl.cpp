#include <curl/curl.h>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/log.h>
#include <common/singleton.h>

#include <syslib/mutex.h>
#include <syslib/thread.h>

#include <network/url_connection.h>

//TODO: custom управление памятью
//TODO: получение данных в паралелльном потоке

using namespace network;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "network.curl"; //название потока протоколирования

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
    CurlStream (const char* url, const char* params, IListener& in_listener)
      : log (LOG_NAME)
      , listener (in_listener)
    {
      try
      {
        CurlLibrarySingleton::Instance ();
        
        stream = curl_easy_init ();
        
        if (!stream)
          throw xtl::format_operation_exception ("", "Can't initialize CURL library");
          
        try
        {
          check_code (curl_easy_setopt (stream, CURLOPT_URL, url), "::curl_easy_setopt");
          check_code (curl_easy_setopt (stream, CURLOPT_WRITEFUNCTION, &WriteDataCallback), "::curl_easy_setopt");
          check_code (curl_easy_setopt (stream, CURLOPT_WRITEDATA, this), "::curl_easy_setopt");
          check_code (curl_easy_perform (stream), "::curl_perform");
          
          listener.FinishReceiveData ();
        }
        catch (...)
        {
          curl_easy_cleanup (stream);
          throw;
        }
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
      curl_easy_cleanup (stream);
    }
    
///Параметры потока
    size_t      GetContentLength   () { return 0; }
    const char* GetContentEncoding () { return "utf-8"; }
    const char* GetContentType     () { return "text"; }
    
///Передача данных
    size_t SendData (const void* buffer, size_t size)
    {
      throw xtl::make_not_implemented_exception ("network::CurlStream::SendData");
    }
    
  private:
///Обработчик получения данных от CURL
    static size_t WriteDataCallback (void *ptr, size_t size, size_t nmemb, void* userdata)
    {
      if (!userdata)
        return 0;
      
      return reinterpret_cast<CurlStream*> (userdata)->WriteReceivedData (ptr, size * nmemb);
    }
    
    size_t WriteReceivedData (void* buffer, size_t size)
    {
      try
      {
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
   
  private:
    common::Log log; //поток протоколирования  
    IListener&  listener; //слушатель событий потока
    CURL*       stream;   //поток CURL
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

}

extern "C"
{

common::ComponentRegistrator<CurlStreamManagerComponent> CurlStreamManager ("network.url_stream.curl");

}
