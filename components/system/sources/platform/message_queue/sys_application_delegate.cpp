#include <platform/message_queue.h>

using namespace syslib;

/*
    Работа с очередью сообщений
*/

namespace
{

class ApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    ApplicationDelegate ()
      : message_queue (*MessageQueueSingleton::Instance ())
    {
      idle_enabled = false;
      is_exited    = false;
      listener     = 0;
            
      message_queue.RegisterHandler (this);
    }
    
///Деструктор
    ~ApplicationDelegate ()
    {
      try
      {
        message_queue.UnregisterHandler (this);
      }
      catch (...)
      {
      }
    }

///Запуск цикла обработки сообщений
    void Run ()
    {
      try
      {
        if (listener)
          listener->OnInitialized ();
        
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
      catch (xtl::exception& e)
      {
        e.touch ("syslib::ApplicationDelegate::Run");
        throw;
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
      {
        message_queue.PushEmptyMessage ();
      }      
    }

///Установка слушателя событий приложения
    void SetListener (IApplicationListener* in_listener)
    {
      listener = in_listener;
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
      ExitMessage (ApplicationDelegate& in_delegate, int in_code) : delegate (in_delegate), code (in_code) {}
      
      void Dispatch ()
      {
        delegate.OnExit (code);
      }
      
      ApplicationDelegate& delegate;
      int                     code;
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
        exception.touch ("syslib::ApplicationDelegate::WaitMessage");
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
        exception.touch ("syslib::ApplicationDelegate::DoNextEvent");
        throw;
      }
    }    

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
    MessageQueue&         message_queue;
};

}

/*
    Создание делегата приложения
*/

namespace syslib
{

IApplicationDelegate* create_message_queue_application_delegate ()
{
  return new ApplicationDelegate;
}

}
