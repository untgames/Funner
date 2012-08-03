#include "shared.h"

using namespace syslib;
using namespace syslib::android;

struct syslib::web_view_handle: public MessageQueue::Handler
{
  IWebViewListener*   listener;               //слушатель событий web-view
  global_ref<jobject> controller;             //контроллер формы
  window_t            window;                 //дескриптор окна
  jmethodID           load_url_method;        //метод загрузки URL ресурса
  jmethodID           load_data_method;       //метод загрузки данных  
  jmethodID           reload_method;          //метод перезагрузки страницы
  jmethodID           stop_loading_method;    //метод перезагрузки страницы  
  jmethodID           can_go_back_method;     //метод проверки возможности возвращени€ назад
  jmethodID           can_go_forward_method;  //метод проверки возможности перемещени€ вперед
  jmethodID           go_back_method;         //метод возвращени€ назад
  jmethodID           go_forward_method;      //метод перемещени€ вперед
  
/// онструктор
  web_view_handle (IWebViewListener* in_listener)
    : listener (in_listener)
    , window ()
  {
    MessageQueueSingleton::Instance ()->RegisterHandler (*this);
  }
  
///ƒеструктор
  ~web_view_handle ()
  {
    MessageQueueSingleton::Instance ()->UnregisterHandler (*this);
  }

///ќбработчик событий окна
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
  
///—обыти€ загрузки
  void OnLoadStartedCallback (const stl::string& request)
  {
    if (listener)
      listener->OnLoadStarted (request.c_str ());
  }

  void OnLoadFinishedCallback ()
  {
    if (listener)
      listener->OnLoadFinished ();
  }

  void OnLoadFailedCallback (const stl::string& error_message)
  {
    if (listener)
      listener->OnLoadFailed (error_message.c_str ());    
  }

///ѕроверка необходимости открыти€ ссылки
  void ShouldStartLoadingCallback (const stl::string& request, const global_ref<jobject>& result)
  {
    bool bool_result = true;
    
    if (listener)
      bool_result = listener->ShouldStartLoading (request.c_str ());

    JNIEnv& env = get_env ();

    local_ref<jclass> result_class = env.GetObjectClass (result.get ());

    if (!result_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed (for UiAsyncResult)");

    jmethodID set_value_method = find_method (&env, result_class.get (), "setValue", "(Z)V");
    
    env.CallVoidMethod (result.get (), set_value_method, (jboolean)bool_result);

    check_errors ();           
  }
};

namespace
{

///ѕолучение дескриптора WebView
web_view_t find_web_view (jobject controller)
{
   try
   {
     if (!controller)
       return 0;

     JNIEnv& env = get_env ();

     local_ref<jclass> controller_class = env.GetObjectClass (controller);

     if (!controller_class)
       throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed (for EngineWebViewController)");
     
     jmethodID get_web_view_ref = env.GetMethodID (controller_class.get (), "getWebViewRef", "()J");

     if (!get_web_view_ref)
       return 0;

     return (web_view_t)check_errors (env.CallLongMethod (controller, get_web_view_ref));
   }
   catch (xtl::exception& e)
   {
     e.touch ("syslib::android::find_web_view");
     throw;
   }
}

template <class Fn> class WebViewMessage: public MessageQueue::Message
{
  public:
    WebViewMessage (web_view_t in_web_view, const Fn& in_fn)
      : web_view (in_web_view)
      , fn (in_fn) {}

    void Dispatch ()
    {
      fn (web_view);
    }

  private:
    web_view_t web_view;
    Fn         fn;
};

template <class Fn> void push_message (jobject controller, const Fn& fn)
{
  try
  {
    web_view_t view = find_web_view (controller);

    if (!view)
      return;

    MessageQueueSingleton::Instance ()->PushMessage (*view, MessageQueue::MessagePtr (new WebViewMessage<Fn> (view, fn), false));
  }
  catch (...)
  {
    //подавление всех исключений
  }
}    

void on_load_started (JNIEnv& env, jobject controller, jstring request)
{
  push_message (controller, xtl::bind (&web_view_handle::OnLoadStartedCallback, _1, tostring (request)));
}

void on_load_finished (JNIEnv& env, jobject controller)
{
  push_message (controller, xtl::bind (&web_view_handle::OnLoadFinishedCallback, _1));
}

void on_load_failed (JNIEnv& env, jobject controller, jstring error_message)
{
  push_message (controller, xtl::bind (&web_view_handle::OnLoadFailedCallback, _1, tostring (error_message)));
}

void should_start_loading (JNIEnv& env, jobject controller, jstring request, jobject result)
{
  push_message (controller, xtl::bind (&web_view_handle::ShouldStartLoadingCallback, _1, tostring (request), global_ref<jobject> (result)));
}

}

/*
    —оздание/уничтожение web-view
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

    view->load_url_method       = find_method (&env, controller_class.get (), "loadUrlThreadSafe", "(Ljava/lang/String;)V");
    view->load_data_method      = find_method (&env, controller_class.get (), "loadDataThreadSafe", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");    
    view->reload_method         = find_method (&env, controller_class.get (), "reloadThreadSafe", "()V");
    view->stop_loading_method   = find_method (&env, controller_class.get (), "stopLoadingThreadSafe", "()V");
    view->can_go_back_method    = find_method (&env, controller_class.get (), "canGoBackThreadSafe", "()Z");
    view->can_go_forward_method = find_method (&env, controller_class.get (), "canGoForwardThreadSafe", "()Z");
    view->go_back_method        = find_method (&env, controller_class.get (), "goBackThreadSafe", "()V");
    view->go_forward_method     = find_method (&env, controller_class.get (), "goForwardThreadSafe", "()V");
    
    jmethodID set_web_view_ref = find_method (&env, controller_class.get (), "setWebViewRef", "(J)V");
    
    env.CallVoidMethod (view->controller.get (), set_web_view_ref, (jlong)&*view);

    check_errors ();    

    return view.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::CreateWebView");
    throw;
  }
}

void AndroidWindowManager::DestroyWebView (web_view_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    web_view_t view = (web_view_t)handle;
    
    AndroidWindowManager::DestroyWindow (view->window);
    
    delete view;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::DestroyWebView");
    throw;
  }
}

/*
    ѕрисоединенное окно
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
    «агрузка данных
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

void AndroidWindowManager::LoadData (web_view_t handle, const char* data, size_t data_size, const char* mime_type, const char* encoding, const char* base_url)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    if (!data && data_size)
      throw xtl::make_null_argument_exception ("", "data");      
      
    if (!data)
      data = "";
      
    if (!mime_type)
      throw xtl::make_null_argument_exception ("", "mime_type");
      
    if (!encoding)
      throw xtl::make_null_argument_exception ("", "encoding");

    if (!base_url)
      throw xtl::make_null_argument_exception ("", "base_url");

    web_view_t view = (web_view_t)handle;

    get_env ().CallVoidMethod (view->controller.get (), view->load_data_method, tojstring (stl::string (data, data + data_size).c_str ()).get (), tojstring (mime_type).get (), tojstring (encoding).get (), tojstring (base_url).get ());
    
    check_errors ();    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::LoadData");
    throw;
  }
}

/*
    ѕерезагрузка страницы / остановка загрузки / проверка наличи€ загрузки
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
    
    get_env ().CallVoidMethod (view->controller.get (), view->stop_loading_method);

    check_errors ();
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
    —редства навигации
*/

bool AndroidWindowManager::CanGoBack (web_view_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    web_view_t view = (web_view_t)handle;

    return check_errors (get_env ().CallBooleanMethod (view->controller.get (), view->can_go_back_method));
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
    
    return check_errors (get_env ().CallBooleanMethod (view->controller.get (), view->can_go_forward_method));
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
    
    get_env ().CallVoidMethod (view->controller.get (), view->go_back_method);

    check_errors ();
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

    get_env ().CallVoidMethod (view->controller.get (), view->go_forward_method);

    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GoForward");
    throw;
  }
}

namespace syslib
{

namespace android
{

/// регистраци€ методов обратного вызова web-view
void register_web_view_callbacks (JNIEnv* env)
{
  try
  {
    if (!env)
      throw xtl::make_null_argument_exception ("", "env");

    jclass controller_class = env->FindClass ("com/untgames/funner/application/EngineWebViewController");

    if (!controller_class)
      throw xtl::format_operation_exception ("", "Can't find EngineWebViewController class\n");

    static const JNINativeMethod methods [] = {
      {"onLoadStarted", "(Ljava/lang/String;)V", (void*)&on_load_started},
      {"onLoadFailed", "(Ljava/lang/String;)V", (void*)&on_load_failed},
      {"onLoadFinished", "()V", (void*)&on_load_finished},
      {"shouldStartLoading", "(Ljava/lang/String;Lcom/untgames/funner/application/UiAsyncBooleanResult;)V", (void*)&should_start_loading},
    };

    static const size_t methods_count = sizeof (methods) / sizeof (*methods);
    
    jint status = env->RegisterNatives (controller_class, methods, methods_count);
    
    if (status)
      throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::register_web_view_callbacks");
    throw;
  }
}

}

}
