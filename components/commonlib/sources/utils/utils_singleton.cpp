#include <common/singleton.h>

using namespace common;

SingletonListNode* SingletonListNode::first             = NULL;
bool               SingletonListNode::atexit_registered = false;

void SingletonListNode::RegisterSingleton (void (*destroy)())
{
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
}

void SingletonListNode::UnregisterSingleton ()
{
  if (prev) prev->next = next;
  if (next) next->prev = prev;
  
  if (this == first)
    first = next;
    
  prev = next = NULL;
}

void SingletonListNode::DestroyAll ()
{
  while (first)
  {
    void (*destroy_function)() = first->destroy_function;

    first->UnregisterSingleton ();

    destroy_function ();
  }  
}

