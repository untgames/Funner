#include <cstdio>
#include <exception>

#include <xtl/function.h>
#include <xtl/common_exceptions.h>

#include <common/action_queue.h>

using namespace common;

const size_t TEST_TIME = 6000;

size_t periodic_counter = 0;
size_t timer_value      = 0;

template <int I> void action_handler ()
{
  switch (I)
  {
    case 1:
//    case 2:
    case 3:
      printf ("  action %d executed\n", I);
      
      if (I == 4)
      {
        periodic_counter++;          
      }
      
      break;
    default:
      throw xtl::format_exception<xtl::bad_operation> ("action_handler", "Exception occured during executing of action %d", I);
  }
}

template <int I> void callback_handler ()
{
  printf ("callback %d\n", I);
}

size_t my_timer ()
{
  return timer_value;
}

int main ()
{
  printf ("Results of action_queue_test:\n");
  
  try
  {
    printf ("perform test\n");
    
    Timer timer (my_timer, 1.0);
    
    static Action actions [] = {
      ActionQueue::PushAction (&action_handler<1>, ActionThread_Current, 0.0, timer),
      ActionQueue::PushAction (&action_handler<3>, &callback_handler<3>, ActionThread_Current, 2.0, timer),
      ActionQueue::PushAction (&action_handler<4>, ActionThread_Current, 2.0, 3.0, timer),
    };

    static const int actions_count = sizeof (actions) / sizeof (*actions);
    
    printf ("heyehey!\n");
    
    for (;timer_value < 10; timer_value++)
    {
      Action action = ActionQueue::PopAction (ActionThread_Current);

      printf ("time=%02u, action:%s%s%s\n", timer_value, action.IsCompleted () ? " completed" : "", action.IsCanceled () ? " canceled" : "",
          action.IsEmpty () ? " empty" : "");
          
      try
      {
        action.Perform ();
      }
      catch (std::exception& e)
      {
        printf ("  exception: %s\n", e.what ());
      }
    }
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
  
  return 0;
}
