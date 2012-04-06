#include <shared/message_queue.h>

using namespace syslib;

static int message_queue_id = 1;

/*
    MessageQueue::Handler
*/

MessageQueue::Handler::Handler ()  
{
  id = xtl::atomic_increment (message_queue_id);
}

MessageQueue::Handler::Handler (const Handler&)
{
  id = xtl::atomic_increment (message_queue_id);
}

/*
    Конструктор / деструктор
*/

MessageQueue::MessageQueue ()
{
}

MessageQueue::~MessageQueue ()
{
  try
  {
    mutex.Lock ();
  }
  catch (...)
  {
  }
}

/*
    Регистрация доступных дескрипторов обработчиков
*/

void MessageQueue::RegisterHandler (const Handler& handler)
{
  try
  {
    Lock lock (mutex);
    
    handlers.insert (handler.Id ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::RegisterHandler");
    throw;
  }
}

void MessageQueue::UnregisterHandler (const Handler& handler)
{
  try
  {
    Lock lock (mutex);
    
    handlers.erase (handler.Id ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::UnregisterHandler");
    throw;
  }
}

/*
    Помещение сообщения в очередь
*/

void MessageQueue::PushMessage (const Handler& handler, const MessagePtr& message)
{
  try
  {
    Lock lock (mutex);
    
    message->id = handler.Id ();
    
    bool is_empty = messages.empty ();
    
    messages.push (message);
    
    if (is_empty)
      condition.NotifyAll ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::PushMessage");
    throw;
  }
}

//используется для пробуждения нити обработки сообщений
void MessageQueue::PushEmptyMessage ()
{
  try
  {
    Lock lock (mutex);
    
    if (messages.empty ())
      condition.NotifyAll ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::PushEmptyMessage");
    throw;
  }
}

/*
    Диспетчеризация первого доступного сообщения его обработчику
*/

void MessageQueue::DispatchFirstMessage ()
{
  try
  {
    MessagePtr message;
    
    {    
      Lock lock (mutex);

      while (messages.empty ())
        condition.Wait (mutex);

      message = messages.front ();
      
      messages.pop ();

      if (!handlers.count (message->id))
        return; //обработчик сообщения не зарегистрирован
    }
    
    if (message)
      message->Dispatch ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::DispatchFirstMessage");
    throw;
  }
}

/*
    Проверка количества сообщений в очереди / ожидание сообщений
*/

bool MessageQueue::IsEmpty () const
{
  try
  {
    Lock lock (const_cast<Mutex&> (mutex));
    
    return messages.empty ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::IsEmpty");
    throw;
  }
}

void MessageQueue::WaitMessage ()
{
  try
  {
    Lock lock (mutex);

    while (messages.empty ())
      condition.Wait (mutex);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::IsEmpty");
    throw;
  }
}
