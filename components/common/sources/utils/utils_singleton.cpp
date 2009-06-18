#include <cstdio>
#include <common/singleton.h>

using namespace common;

SingletonListNode*       SingletonListNode::first             = 0;
bool                     SingletonListNode::atexit_registered = false;
SingletonLog::LogHandler SingletonLog::log_handler            = 0;

namespace
{

Lockable singleton_service_lockable;

}

/*
    SingletonListNode    
*/

void SingletonListNode::RegisterSingleton (const std::type_info& in_type, void (*destroy)())
{
  Lock lock (singleton_service_lockable);

  if (!atexit_registered)
  {
    atexit (DestroyAll);
    atexit_registered = true;
  }

  prev  = NULL;
  next  = first;
  first = this;

  if (next)
    next->prev = this;

  destroy_function = destroy;
  type             = &in_type;
}

void SingletonListNode::UnregisterSingleton ()
{
  Lock lock (singleton_service_lockable);

  if (prev) prev->next = next;
  if (next) next->prev = prev;

  if (this == first)
    first = next;

  prev = next = NULL;
}

void SingletonListNode::DestroyAll ()
{
  Lock lock (singleton_service_lockable);

  while (first)
  {
    void (*destroy_function)() = first->destroy_function;

    first->UnregisterSingleton ();
    
    destroy_function ();
  }
}

/*
    SingletonLog
*/

void SingletonLog::LogEvent (Event event, const std::type_info& type)
{
  Lock lock (singleton_service_lockable);
  
  if (!log_handler)
    return;
    
  log_handler (event, type);
}
    
void SingletonLog::SetLogHandler (LogHandler handler)
{
  Lock lock (singleton_service_lockable);
  
  log_handler = handler;
}

SingletonLog::LogHandler SingletonLog::GetLogHandler ()
{
  Lock lock (singleton_service_lockable);
  
  return log_handler;
}

void SingletonLog::DebugLogHandler (Event event, const std::type_info& type)
{
  const char* event_message = "";

  switch (event)
  {
    case SingletonLog::Event_BeforeCreate:  event_message = "before create";  break;
    case SingletonLog::Event_AfterCreate:   event_message = "after create";   break;
    case SingletonLog::Event_BeforeDestroy: event_message = "before destroy"; break;
    case SingletonLog::Event_AfterDestroy:  event_message = "after destroy";  break;
    default: break; //for gcc
  }

  printf ("%s '%s'\n", event_message, type.name ());
}
