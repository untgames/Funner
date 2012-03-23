#include "shared.h"

using namespace syslib;

namespace
{

class AndroidApplicationDelegate;

AndroidApplicationDelegate* volatile application_delegate = 0;

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

template <class Fn> void push_message (const Fn& fn)
{
  try
  {
    MessageQueueSingleton::Instance ()->PushMessage (application_delegate, MessageQueue::MessagePtr (new ActivityMessage<Fn> (fn), false));
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

class AndroidApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    AndroidApplicationDelegate ()
      : message_queue (*MessageQueueSingleton::Instance ())    
    {
      if (application_delegate)
        throw xtl::format_operation_exception ("syslib::android::AndroidApplicationDelegate::AndroidApplicationDelegate", "Delegate has been already created");
      
      idle_enabled = false;
      is_exited    = false;
      is_paused    = false;
      listener     = 0;
      
      MessageQueueSingleton::Instance ()->RegisterHandler (this);
      
      application_delegate = this;      
    }
    
///Деструктор
    ~AndroidApplicationDelegate ()
    {
      application_delegate = 0;
      
      try
      {
        MessageQueueSingleton::Instance ()->UnregisterHandler (this);
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
      message_queue.PushMessage (this, MessageQueue::MessagePtr (new ExitMessage (*this, code), false));      
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
      printf ("activity state %d\n", state); fflush (stdout);
      
      {
        Lock lock (pause_mutex);
        
        bool in_is_paused = !state;
        
        if (in_is_paused == is_paused)
          return;
          
        is_paused = in_is_paused;
        
        pause_condition.NotifyOne ();
      }
      
      if (state)
      {
        push_message (xtl::bind (&AndroidApplicationDelegate::OnResume, this));
      }
      else
      {
        push_message (xtl::bind (&AndroidApplicationDelegate::OnPause, this));        
      }
    }
    
///Оповещение о приостановке приложения
    void OnPause ()
    {        
      if (listener)
        listener->OnPause ();
        
      WaitActivityResume ();        
    }
    
///Оповещение о восстановлении приложения
    void OnResume ()
    {
      if (listener)
        listener->OnResume ();
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
    
///Ожидание активности приложения
    void WaitActivityResume ()
    {
      Lock lock (pause_mutex);
      
      while (is_paused)
        pause_condition.Wait (pause_mutex);
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
    bool                  idle_enabled;
    bool                  is_exited;
    volatile bool         is_paused;
    Mutex                 pause_mutex;
    Condition             pause_condition;
    IApplicationListener* listener;
    MessageQueue&         message_queue;    
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
    
  application_delegate->SetActivityState (true);
}

void on_low_memory_callback (JNIEnv& env, jobject activity)
{
  if (!application_delegate)
    return;

  push_message (xtl::bind (&AndroidApplicationDelegate::OnMemoryWarning, application_delegate));
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
  throw xtl::make_not_implemented_exception ("syslib::AndroidApplicationManager::OpenUrl");
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
