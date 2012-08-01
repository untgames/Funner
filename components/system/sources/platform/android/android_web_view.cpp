#include "shared.h"

using namespace syslib;
using namespace syslib::android;

struct syslib::web_view_handle
{
  IWebViewListener*   listener;         //слушатель событий web-view
  global_ref<jobject> controller;       //контроллер формы
  window_t            window;           //дескриптор окна
  jmethodID           load_url_method;  //метод загрузки URL ресурса
  jmethodID           reload_method;    //метод перезагрузки страницы
  
///Конструктор
  web_view_handle (IWebViewListener* in_listener)
    : listener (in_listener)
    , window ()
  {
  }

///Обработчик событий окна
  static void WindowMessageHandler (window_t, WindowEvent event, const WindowEventContext& context, void* user_data)
  {
    if (!user_data)
      return;
      
    reinterpret_cast<web_view_handle*> (user_data)->ProcessWindowMessage (event, context);
  }
  
  void ProcessWindowMessage (WindowEvent event, const WindowEventContext&)
  {
    switch (event)
    {
      case WindowEvent_OnDestroy:
        if (listener)
          listener->OnClose ();

        break;
      default:
        break;
    }
  }  
};

/*
    Создание/уничтожение web-view
*/

web_view_t AndroidWindowManager::CreateWebView (IWebViewListener* listener)
{
  try
  {
      //создание формы
    stl::auto_ptr<web_view_handle> view (new web_view_handle (listener));    

    void* view_controller = 0;

    view->window = AndroidPlatform::CreateWindow (WindowStyle_PopUp, &web_view_handle::WindowMessageHandler, 0, 0, (void*)&*view, AndroidWindowManager::WindowType_WebView, &view_controller);

    if (!view->window || !view_controller)
      throw xtl::format_operation_exception ("", "AndroidPlatform::CreateWindow failed (null result)");

    view->controller = global_ref<jobject> ((jobject)view_controller);
    
    JNIEnv& env = get_env ();

    local_ref<jclass> controller_class = env.GetObjectClass (view->controller.get ());
        
    if (!controller_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed (for EngingeViewController)");

    view->load_url_method = find_method (&env, controller_class.get (), "loadUrlThreadSafe", "(Ljava/lang/String;)V");
    view->reload_method   = find_method (&env, controller_class.get (), "reloadThreadSafe", "()V");

    return view.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::CreateWebView");
    throw;
  }
}

void AndroidWindowManager::DestroyWebView (web_view_t)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::DestroyWebView");
    throw;
  }
}

/*
    Присоединенное окно
*/

window_t AndroidWindowManager::GetWindow (web_view_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    web_view_t view = (web_view_t)handle;
    
    return view->window;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetWindow");
    throw;
  }
}

/*
    Загрузка данных
*/

void AndroidWindowManager::LoadRequest (web_view_t handle, const char* request)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");      
    
    if (!request)
      throw xtl::make_null_argument_exception ("", "request");
      
    web_view_t view = (web_view_t)handle;

    get_env ().CallVoidMethod (view->controller.get (), view->load_url_method, tojstring (request).get ());
    
    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::LoadRequest");
    throw;
  }
}

void AndroidWindowManager::LoadData (web_view_t handle, const char*, size_t, const char*, const char*, const char*)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    web_view_t view = (web_view_t)handle;
    
    
    throw xtl::make_not_implemented_exception (__FUNCTION__);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::LoadData");
    throw;
  }
}

/*
    Перезагрузка страницы / остановка загрузки / проверка наличия загрузки
*/

void AndroidWindowManager::Reload (web_view_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    web_view_t view = (web_view_t)handle;

    get_env ().CallVoidMethod (view->controller.get (), view->reload_method);

    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::Reload");
    throw;
  }
}

void AndroidWindowManager::StopLoading (web_view_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    web_view_t view = (web_view_t)handle;
    
    
    throw xtl::make_not_implemented_exception (__FUNCTION__);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::StopLoading");
    throw;
  }
}

bool AndroidWindowManager::IsLoading (web_view_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    web_view_t view = (web_view_t)handle;
    
    
    throw xtl::make_not_implemented_exception (__FUNCTION__);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::IsLoading");
    throw;
  }
}

/*
    Средства навигации
*/

bool AndroidWindowManager::CanGoBack (web_view_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    web_view_t view = (web_view_t)handle;
    
    
    throw xtl::make_not_implemented_exception (__FUNCTION__);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::CanGoBack");    
    throw;
  }
}

bool AndroidWindowManager::CanGoForward (web_view_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    web_view_t view = (web_view_t)handle;
    
    
    throw xtl::make_not_implemented_exception (__FUNCTION__);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::CanGoForward");
    throw;
  }
}

void AndroidWindowManager::GoBack (web_view_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    web_view_t view = (web_view_t)handle;
    
    
    throw xtl::make_not_implemented_exception (__FUNCTION__);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GoBack");
    throw;
  }
}

void AndroidWindowManager::GoForward (web_view_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    web_view_t view = (web_view_t)handle;
    
    
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GoForward");
    throw;
  }
}
