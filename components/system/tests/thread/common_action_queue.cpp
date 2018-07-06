#include <cstdio>
#include <exception>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <common/action_queue.h>

#include <syslib/application.h>

#include <syslib/application.h>

using namespace common;

template <int I> void action_handler (Action& action)
{
  printf ("  action %d executed\n", I);
}

template <int I> void callback_handler ()
{
  printf ("  callback %d\n", I);
}

bool my_wait_handler (Action& action, size_t milliseconds)
{
  printf ("  wait %u milliseconds\n", milliseconds);
  
  return action.IsCompleted ();
}

template <ActionQueueEvent event> void event_handler (ActionThread thread, Action& action)
{
  switch (event)
  {
    case ActionQueueEvent_OnPushAction:
      printf ("push action to thread %d\n", thread);
      action.SetWaitHandler (&my_wait_handler);
      break;
    case ActionQueueEvent_OnPopAction:
      printf ("pop action from thread %d\n", thread);
      break;
    default:
      printf ("unknown event in thread %d\n", thread);
      break;
  }
}

int main ()
{
  printf ("Results of action_queue_test:\n");
  
  try
  {
    ActionQueue::RegisterEventHandler (ActionQueueEvent_OnPushAction, &event_handler<ActionQueueEvent_OnPushAction>);
    ActionQueue::RegisterEventHandler (ActionQueueEvent_OnPopAction, &event_handler<ActionQueueEvent_OnPopAction>);
    
    printf ("-- perform test --\n");
    
    ActionQueue::PushAction (&action_handler<1>, &callback_handler<1>, ActionThread_Current, common::ActionQueue::time_t ());
    ActionQueue::PushAction (&action_handler<2>, ActionThread_Current, common::ActionQueue::time_t (2));
    ActionQueue::PushAction (&action_handler<3>, ActionThread_Current, common::ActionQueue::time_t (2), common::ActionQueue::time_t (3));
    ActionQueue::PushAction (&action_handler<4>, ActionThread_Current, common::ActionQueue::time_t (5, 10), common::ActionQueue::time_t (15, 10));

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
  
  return 0;
}
