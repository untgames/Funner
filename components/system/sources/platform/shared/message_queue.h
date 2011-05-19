#ifndef SYSLIB_MESSAGE_QUEUE_INTERNAL_HEADER
#define SYSLIB_MESSAGE_QUEUE_INTERNAL_HEADER

#include <stl/hash_set>
#include <stl/queue>

#include <xtl/exception.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/singleton.h>

#include <syslib/condition.h>
#include <syslib/mutex.h>
#include <syslib/application.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очередь сообщений приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
class MessageQueue
{
  public:
    typedef void* handler_t;
      
///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс сообщения
///////////////////////////////////////////////////////////////////////////////////////////////////
    class Message: public xtl::reference_counter
    {
      friend class MessageQueue;
      public:
        Message () : handler (0) {}
      
        virtual ~Message () {}      
      
        virtual void Dispatch () = 0;
        
      private:
        handler_t handler;
    };
    
    typedef xtl::intrusive_ptr<Message> MessagePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    MessageQueue  ();
    ~MessageQueue ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация доступных дескрипторов обработчиков
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterHandler   (handler_t);
    void UnregisterHandler (handler_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Помещение сообщения в очередь
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PushMessage      (handler_t, const MessagePtr& message);
    void PushEmptyMessage (); //используется для пробуждения нити обработки сообщений
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Диспетчеризация первого доступного сообщения его обработчику
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void DispatchFirstMessage ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка количества сообщений в очереди / ожидание сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////    
    bool IsEmpty     () const;
    void WaitMessage ();
    
  private:
    MessageQueue (const MessageQueue&); //no impl
    MessageQueue& operator = (const MessageQueue&); //no impl
    
  private:
    typedef stl::queue<MessagePtr>   MessageQueueImpl;
    typedef stl::hash_set<handler_t> HandlerSet;

  private:
    Mutex            mutex;
    Condition        condition;
    MessageQueueImpl messages;
    HandlerSet       handlers;
};

typedef common::Singleton<MessageQueue> MessageQueueSingleton;

IApplicationDelegate* create_message_queue_application_delegate ();

}

#endif
