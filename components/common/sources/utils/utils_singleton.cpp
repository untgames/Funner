#include <common/singleton.h>

using namespace common;

SingletonListNode*       SingletonListNode::first             = 0;
bool                     SingletonListNode::atexit_registered = false;
SingletonLog::LogHandler SingletonLog::log_handler            = 0;

namespace
{

struct SingletonServiceLockable: public xtl::singleton_default<SingletonServiceLockable>
{
  Lockable lockable;
};

}

/*
    SingletonListNode    
*/

void SingletonListNode::RegisterSingleton (const std::type_info& in_type, void (*destroy)())
{
  Lock lock (SingletonServiceLockable::instance ().lockable);

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
  Lock lock (SingletonServiceLockable::instance ().lockable);

  if (prev) prev->next = next;
  if (next) next->prev = prev;

  if (this == first)
    first = next;

  prev = next = NULL;
}

void SingletonListNode::DestroyAll ()
{
  Lock lock (SingletonServiceLockable::instance ().lockable);

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
  Lock lock (SingletonServiceLockable::instance ().lockable);
  
  if (!log_handler)
    return;
    
  log_handler (event, type);
}
    
void SingletonLog::SetLogHandler (LogHandler handler)
{
  Lock lock (SingletonServiceLockable::instance ().lockable);
  
  log_handler = handler;
}

SingletonLog::LogHandler SingletonLog::GetLogHandler ()
{
  Lock lock (SingletonServiceLockable::instance ().lockable);
  
  return log_handler;
}
