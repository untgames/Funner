#include <platform/message_queue.h>

using namespace syslib;

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

void MessageQueue::RegisterHandler (handler_t handler)
{
  try
  {
    Lock lock (mutex);
    
    handlers.insert (handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MessageQueue::RegisterHandler");
    throw;
  }
}

void MessageQueue::UnregisterHandler (handler_t handler)
{
  try
  {
    Lock lock (mutex);
    
    handlers.erase (handler);
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

void MessageQueue::PushMessage (handler_t handler, const MessagePtr& message)
{
  try
  {
    Lock lock (mutex);
    
    message->handler = handler;
    
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
    Lock lock (mutex);

    while (messages.empty ())
      condition.Wait (mutex);

    MessagePtr message = messages.front ();
    
    messages.pop ();

    if (!handlers.count (message->handler))
      return; //обработчик сообщения не зарегистрирован

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
