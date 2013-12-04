#include "shared.h"

using namespace syslib;
using namespace syslib::android;

namespace
{

const char* LOG_NAME = "syslib.android";

class AndroidApplicationDelegate;

AndroidApplicationDelegate* volatile application_delegate = 0;

static bool                      screen_saver_state = true;
static ApplicationBackgroundMode background_mode    = ApplicationBackgroundMode_Suspend;

template <class Fn> class ActivityMessage: public MessageQueue::Message
{
  public:
    ActivityMessage (const Fn& in_fn)
      : fn (in_fn) {}

    void Dispatch ()
    {
      fn ();
    }

  private:
    Fn fn;
};

class AndroidApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter, public MessageQueue::Handler
{
  public:
///Конструктор
    AndroidApplicationDelegate ()
      : background_mode_after_pause (ApplicationBackgroundMode_Suspend)
      , message_queue (*MessageQueueSingleton::Instance ())    
      , log (LOG_NAME)
    {
      if (application_delegate)
        throw xtl::format_operation_exception ("syslib::android::AndroidApplicationDelegate::AndroidApplicationDelegate", "Delegate has been already created");
      
      idle_enabled = false;
      is_exited    = false;
      is_paused    = false;
      listener     = 0;
      
      MessageQueueSingleton::Instance ()->RegisterHandler (*this);
      
      application_delegate = this;      
    }
    
///Деструктор
    ~AndroidApplicationDelegate ()
    {
      application_delegate = 0;
      
      try
      {
        MessageQueueSingleton::Instance ()->UnregisterHandler (*this);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }

///Помещение сообщения в очередь
    template <class Fn> static void PushMessage (const Fn& fn)
    {
      try
      {
        MessageQueueSingleton::Instance ()->PushMessage (*application_delegate, MessageQueue::MessagePtr (new ActivityMessage<Fn> (fn), false));
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }
    
///Запуск цикла обработки сообщений
    void Run ()
    {
      if (listener)
        listener->OnInitialize ();
      
      while (!is_exited)
      {
        while (!IsMessageQueueEmpty ())
          DoNextEvent ();                    

         //если нет обработчиков OnIdle - приостанавливаем приложение

        if (!idle_enabled)
        {
          if (IsMessageQueueEmpty () && !is_exited)
          {
            WaitMessage ();
          }
        }
        else
        {
          if (listener)
            listener->OnIdle ();
        }        
      }
    }

///Выход из приложения
    void Exit (int code)
    {
      message_queue.PushMessage (*this, MessageQueue::MessagePtr (new ExitMessage (*this, code), false));      
    }
    
///Обработка события выхода из приложения
    void OnExit (int code)
    {
      is_exited = true;
      
      if (listener)
        listener->OnExit (code);
    }    

///Установка необходимости вызова событий idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
      
      if (state)
        message_queue.PushEmptyMessage ();
    }

///Установка слушателя событий приложения
    void SetListener (IApplicationListener* in_listener)
    {
      listener = in_listener;
    }
    
///Установка состояния активности приложения
    void SetActivityState (bool state)
    {
      {
        Lock lock (pause_mutex);
        
        bool in_is_paused = !state;
        
        if (in_is_paused == is_paused)
          return;
          
        is_paused = in_is_paused;

        if (in_is_paused)
          background_mode_after_pause = background_mode;
        
        pause_condition.NotifyOne ();
      }

      if (!state)
      {
        PushMessage (xtl::bind (&AndroidApplicationDelegate::OnPause, this));
      }
      else
      {
        if (background_mode_after_pause == ApplicationBackgroundMode_Active)
          PushMessage (xtl::bind (&AndroidApplicationDelegate::OnResumeInActiveMode, this));
      }
    }
    
///Оповещение о приостановке приложения
    void OnPause ()
    {        
      if (listener)
        listener->OnPause ();
        
      log.Printf ("Application paused");

      if (background_mode_after_pause == ApplicationBackgroundMode_Active)
        return;       

      {
        Lock lock (pause_mutex);

        while (is_paused)
          pause_condition.Wait (pause_mutex);        
      }

      OnResume ();
    }

    void OnResume ()
    {
      log.Printf ("Application resumed");

      if (listener)
        listener->OnResume ();
    }

///Оповещение о возврате приложения из сна (вызывается только в случае ApplicationBackgroundMode_Active)
    void OnResumeInActiveMode ()
    {
      OnResume ();
    }

///Оповещение о недостаточности памяти
    void OnMemoryWarning ()
    {
      if (listener)
        listener->OnMemoryWarning ();
    }
    
///Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }
    
    void Release ()
    {
      release (this);
    }
    
  private:
///Отложенное события выхода из приложения
    struct ExitMessage: public MessageQueue::Message
    {
      ExitMessage (AndroidApplicationDelegate& in_delegate, int in_code) : delegate (in_delegate), code (in_code) {}
      
      void Dispatch ()
      {
        delegate.OnExit (code);
      }
      
      AndroidApplicationDelegate& delegate;
      int                         code;
    };
    
///Проверка очереди событий на пустоту
    bool IsMessageQueueEmpty ()
    {
      if (is_exited)
        return true;
        
      return message_queue.IsEmpty ();
    }    
    
///Ожидание события
    void WaitMessage ()
    {
      try
      {
        message_queue.WaitMessage ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::android::AndroidApplicationDelegate::WaitMessage");
        throw;
      }
    }
    
///Обработка следующего события
    void DoNextEvent ()
    {
      try
      {
        message_queue.DispatchFirstMessage ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::android::AndroidApplicationDelegate::DoNextEvent");
        throw;
      }
    }

  private:
    bool                      idle_enabled;
    bool                      is_exited;
    volatile bool             is_paused;
    Mutex                     pause_mutex;
    Condition                 pause_condition;
    ApplicationBackgroundMode background_mode_after_pause;
    IApplicationListener*     listener;
    MessageQueue&             message_queue;
    common::Log               log;
};

/*
    Оповещения о событиях приложения
*/

void on_pause_callback (JNIEnv& env, jobject activity)
{
  if (!application_delegate)
    return;

  application_delegate->SetActivityState (false);
}

void on_resume_callback (JNIEnv& env, jobject activity)
{
  if (!application_delegate)
    return;
    
  set_activity (activity);

  application_delegate->SetActivityState (true);
}

void on_low_memory_callback (JNIEnv& env, jobject activity)
{
  if (!application_delegate)
    return;

  AndroidApplicationDelegate::PushMessage (xtl::bind (&AndroidApplicationDelegate::OnMemoryWarning, application_delegate));
}

/*
    Посылка сообщения приложению
*/

void post_notification_impl (const stl::string& notification)
{
  Application::PostNotification (notification.c_str ());
}

void post_notification (JNIEnv& env, jobject, jstring notification)
{
  AndroidApplicationDelegate::PushMessage (xtl::bind (&post_notification_impl, tostring (notification)));
}

}

/*
    Создание делегата приложения
*/

IApplicationDelegate* AndroidApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new AndroidApplicationDelegate;
}

/*
    Открытие URL во внешнем браузере
*/

void AndroidApplicationManager::OpenUrl (const char* url)
{
  try
  {
    if (!url)
      throw xtl::make_null_argument_exception ("", "url");

    local_ref<jobject> activity (get_activity ());
    local_ref<jclass>  activity_class (get_env ().GetObjectClass (get_activity ()), false);

    if (!activity_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");

    jmethodID open_url_method = find_method (&get_env (), activity_class.get (), "openUrl", "(Ljava/lang/String;)V");

    get_env ().CallVoidMethod (activity.get (), open_url_method, tojstring (url).get ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidApplicationManager::OpenUrl");
    throw;
  }
}

/*
    Управление энергосбережением
*/

void AndroidApplicationManager::SetScreenSaverState (bool state)
{
  try
  {
    local_ref<jobject> activity (get_activity ());
    local_ref<jclass>  activity_class (get_env ().GetObjectClass (get_activity ()), false);

    if (!activity_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");

    jmethodID set_screen_saver_state_method = find_method (&get_env (), activity_class.get (), "setScreenSaverStateThreadSafe", "(Z)V");

    get_env ().CallVoidMethod (activity.get (), set_screen_saver_state_method, state);

    screen_saver_state = state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidApplicationManager::SetScreenSaverState");
    throw;
  }
}

bool AndroidApplicationManager::GetScreenSaverState ()
{
  return screen_saver_state;
}

/*
    Получение системных свойств
*/

void AndroidApplicationManager::GetSystemProperties (common::PropertyMap& properties)
{
  try
  {
    local_ref<jobject> activity (get_activity ()); 
    local_ref<jclass>  activity_class (get_env ().GetObjectClass (get_activity ()), false);

    if (!activity_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");

    jmethodID get_system_properties_method = find_method (&get_env (), activity_class.get (), "getSystemProperties", "()Ljava/lang/String;");

    local_ref<jstring> properties_line_jstring ((jstring)get_env ().CallObjectMethod (activity.get (), get_system_properties_method), false);

    stl::string properties_line = tostring (properties_line_jstring.get ());

    common::PropertyMap android_properties = common::parse_init_string (properties_line.c_str ());

    properties.SetProperty ("Operating System", "Android");

    for (size_t i=0, count=android_properties.Size (); i<count; i++)
      properties.SetProperty (android_properties.PropertyName (i), android_properties.GetString (i));
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidApplicationManager::GetSystemProperties");
    throw;
  }
}

/*
    Управление режимом работы в фоне
*/

void AndroidApplicationManager::SetApplicationBackgroundMode (syslib::ApplicationBackgroundMode mode)
{
  background_mode = mode;

  if (background_mode == ApplicationBackgroundMode_Active && application_delegate)
    application_delegate->SetActivityState (true);    
}

ApplicationBackgroundMode AndroidApplicationManager::GetApplicationBackgroundMode ()
{
  return background_mode;
}

namespace syslib
{

namespace android
{

/// регистрация методов обратного вызова activity
void register_activity_callbacks (JNIEnv* env, jclass activity_class)
{
  try
  {
    if (!env)
      throw xtl::make_null_argument_exception ("", "env");

    if (!activity_class)
      throw xtl::make_null_argument_exception ("", "activity_class");

    static const JNINativeMethod methods [] = {
      {"onPauseCallback", "()V", (void*)&on_pause_callback},
      {"onResumeCallback", "()V", (void*)&on_resume_callback},
      {"onLowMemoryCallback", "()V", (void*)&on_low_memory_callback},
      {"postNotification", "(Ljava/lang/String;)V", (void*)&post_notification},
    };

    static const size_t methods_count = sizeof (methods) / sizeof (*methods);

    jint status = env->RegisterNatives (activity_class, methods, methods_count);
    
    if (status)
      throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::register_activity_callbacks");
    throw;
  }
}

}

}
