#include <cstdio>
#include <exception>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <common/action_queue.h>

using namespace common;

size_t timer_value = 0;

template <int I> void action_handler (Action& action)
{
  switch (I)
  {
    case 1:
//    case 2:
    case 3:
      printf ("  action %d executed\n", I);
      
      if (I == 3)
      {
        static size_t periodic_counter = 0;
        
        if (periodic_counter++ == 2)
          action.Cancel ();
      }
      
      break;
    default:
      throw xtl::format_exception<xtl::bad_operation> ("action_handler", "Exception occured during executing of action %d", I);
  }
}

template <int I> void callback_handler ()
{
  printf ("  callback %d\n", I);
}

size_t my_timer ()
{
  return timer_value;
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
    
    Timer timer (my_timer, 1.0);
    
    timer.Start ();
    
    ActionQueue::PushAction (&action_handler<1>, &callback_handler<1>, ActionThread_Current, 0.0, timer);
    ActionQueue::PushAction (&action_handler<2>, ActionThread_Current, 2.0, timer);
    ActionQueue::PushAction (&action_handler<3>, ActionThread_Current, 2.0, 3.0, timer);

    for (;timer_value < 10; timer_value++)
    {
      Action action = ActionQueue::PopAction (ActionThread_Current);

      printf ("time=%02u, action:%s%s%s\n", timer_value, action.IsCompleted () ? " completed" : "", action.IsCanceled () ? " canceled" : "",
          action.IsEmpty () ? " empty" : "");
          
      if (action.IsEmpty ())
        continue;
          
      try
      {
        action.Perform ();
        
        printf ("  wait: %s\n", action.Wait (100) ? "true" : "false");
      }
      catch (std::exception& e)
      {
        printf ("  exception: %s\n", e.what ());
      }
      
      printf ("  after perform action:%s%s%s\n", action.IsCompleted () ? " completed" : "", action.IsCanceled () ? " canceled" : "",
          action.IsEmpty () ? " empty" : "");
    }
    
    printf ("-- cleanup current thread --\n");
    
    ActionQueue::CleanupCurrentThread ();
    
    printf ("exit\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
  
  return 0;
}
