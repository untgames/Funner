#include "shared.h"

using namespace syslib;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "system.web_view";

}

/*
    Описание реализации WebView
*/

typedef xtl::signal<void (WebView& view, WebViewEvent event)>  EventSignal;
typedef xtl::signal<bool (WebView& view, const char* request)> FilterSignal;

struct WebView::Impl: public IWebViewListener
{
  WebView*       owner;                     //обратная ссылка на владельца
  web_view_t     handle;                    //дескриптор области вывода
  syslib::Window window;                    //присоединенное окно
  FilterSignal   filters;                   //фильтры
  EventSignal    events [WebViewEvent_Num]; //события
  stl::string    status;                    //статус
  stl::string    request;                   //запрос
  common::Log    log;                       //протокол отладочных сообщений

///Конструктор
  Impl (WebView* in_owner)
    : owner (in_owner)
    , handle (Platform::CreateWebView (this))
    , window (Platform::GetWindow (handle))
    , log (LOG_NAME)
  {
  }

///Деструктор
  ~Impl ()
  {
    try
    {            
      Platform::DestroyWebView (handle);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at syslib::WebView::Impl::~Impl", e.what ());
    }    
    catch (...)
    {
      log.Printf ("Unknown exception\n    at syslib::WebView::Impl::~Impl");      
    }
  }

///События загрузки
  void OnLoadStarted (const char* request)
  {
    try
    {
      if (!request)
        throw xtl::make_null_argument_exception ("", "request");
          
      this->request = request;
      status        = "Loading";
      
      events [WebViewEvent_OnLoadStart] (*owner, WebViewEvent_OnLoadStart);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at syslib::WebView::Impl::OnLoadStarted", e.what ());
    }    
    catch (...)
    {
      log.Printf ("Unknown exception\n    at syslib::WebView::Impl::OnLoadStarted");
    }
  }
  
  void OnLoadFinished ()
  {
    try
    {
      status = "Loaded";
      
      events [WebViewEvent_OnLoadFinish] (*owner, WebViewEvent_OnLoadFinish);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at syslib::WebView::Impl::OnLoadFinished", e.what ());
    }    
    catch (...)
    {
      log.Printf ("Unknown exception\n    at syslib::WebView::Impl::OnLoadFinished");
    }
  }
  
  void OnLoadFailed (const char* error_message)
  {
    try
    {
      if (!error_message)
        error_message = "unknown error";
      
      status = common::format ("Load failed: %s", error_message);
      
      events [WebViewEvent_OnLoadFail] (*owner, WebViewEvent_OnLoadFail);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at syslib::WebView::Impl::OnLoadFailed", e.what ());
    }    
    catch (...)
    {
      log.Printf ("Unknown exception\n    at syslib::WebView::Impl::OnLoadFailed");
    }
  }
  
///События взаимодействия с окном
  void OnClose ()
  {
    try
    {
      status = "Closed";
      
      events [WebViewEvent_OnClose] (*owner, WebViewEvent_OnClose);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at syslib::WebView::Impl::OnClose", e.what ());
    }    
    catch (...)
    {
      log.Printf ("Unknown exception\n    at syslib::WebView::Impl::OnClose");
    }    
  }

///Проверка необходимости открытия ссылки
  bool ShouldStartLoading (const char* request)
  {
    try
    {
      if (!request)
        throw xtl::make_null_argument_exception ("", "request");              
          
      return filters.empty () ? true : filters (*owner, request);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at syslib::WebView::Impl::ShouldStartLoading", e.what ());
      
      return true;
    }    
    catch (...)
    {
      log.Printf ("Unknown exception\n    at syslib::WebView::Impl::ShouldStartLoading");
      
      return true;
    }
  }
};

/*
    Конструктор / деструктор
*/

WebView::WebView ()
{
  try
  {
    impl = new Impl (this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WebView::WebView");
    throw;
  }
}

WebView::~WebView ()
{
  try
  {            
    impl->events [WebViewEvent_OnDestroy] (*this, WebViewEvent_OnDestroy);
  }
  catch (std::exception& e)
  {
    impl->log.Printf ("%s\n    at syslib::WebView::~WebView", e.what ());
  }    
  catch (...)
  {
    impl->log.Printf ("Unknown exception\n    at syslib::WebView::~WebView");      
  }
}

/*
    Присоединенное окно
*/

const syslib::Window& WebView::Window () const
{
  return impl->window;
}

syslib::Window& WebView::Window ()
{
  return impl->window;
}

/*
    Загрузка данных
*/

void WebView::LoadRequest (const char* uri)
{
  try
  {
    if (!uri)
      throw xtl::make_null_argument_exception ("", "uri");
      
    Platform::LoadRequest (impl->handle, uri);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WebView::LoadRequest");
    throw;
  }
}

void WebView::LoadData (const char* data, size_t data_size, const char* mime_type, const char* encoding, const char* base_url)
{
  try
  {
    if (data_size && !data)
      throw xtl::make_null_argument_exception ("", "data");
      
    if (!mime_type)
      throw xtl::make_null_argument_exception ("", "mime_type");
      
    if (!encoding)
      throw xtl::make_null_argument_exception ("", "encoding");
      
    if (!base_url)
      throw xtl::make_null_argument_exception ("", "base_url");
    
    Platform::LoadData (impl->handle, data, data_size, mime_type, encoding, base_url);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WebView::LoadData");
    throw;
  }
}

/*
    Перезагрузка страницы / остановка загрузки / проверка наличия загрузки
*/

void WebView::Reload ()
{
  try
  {
    Platform::Reload (impl->handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WebView::Reload");
    throw;
  }
}

void WebView::StopLoading ()
{
  try
  {
    Platform::StopLoading (impl->handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WebView::StopLoading");
    throw;
  }
}

bool WebView::IsLoading () const
{
  try
  {
    return Platform::IsLoading (impl->handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WebView::IsLoading");
    throw;
  }
}

/*
    Значение текущего запроса / статус выполнения запроса
*/

const char* WebView::Request () const
{
  return impl->request.c_str ();
}

const char* WebView::Status () const
{
  return impl->status.c_str ();
}

/*
    Средства навигации
*/

bool WebView::CanGoBack () const
{
  try
  {
    return Platform::CanGoBack (impl->handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WebView::CanGoBack");
    throw;
  }
}

bool WebView::CanGoForward () const
{
  try
  {
    return Platform::CanGoForward (impl->handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WebView::CanGoForward");
    throw;
  }
}

void WebView::GoBack ()
{
  try
  {
    Platform::GoBack (impl->handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WebView::GoBack");
    throw;
  }
}

void WebView::GoForward ()
{
  try
  {
    Platform::GoForward (impl->handle);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WebView::GoForward");
    throw;
  }
}

/*
    Фильтр контента
*/

xtl::connection WebView::RegisterFilter (const Filter& filter) const
{
  return impl->filters.connect (filter);
}

/*
    События
*/

xtl::connection WebView::RegisterEventHandler (WebViewEvent event, const EventHandler& handler) const
{
  switch (event)
  {
    case WebViewEvent_OnLoadStart:
    case WebViewEvent_OnLoadFail:
    case WebViewEvent_OnLoadFinish:
    case WebViewEvent_OnClose:
    case WebViewEvent_OnDestroy:
      break;
    default:
      throw xtl::make_argument_exception ("syslib::WebView::RegisterEventHandler", "event", event);
  }
  
  return impl->events [event].connect (handler);
}
