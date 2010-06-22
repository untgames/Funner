#include "shared.h"

const size_t TEST_TIME = 10*1000;

size_t get_clamped_time ()
{
  return common::milliseconds () / 1000 * 1000;
}

template <int I> void action_handler (Action& action)
{
  switch (I)
  {
    case 1:
//    case 2:
    case 3:
      printf ("[%08u] action %d executed\n", get_clamped_time (), I);
      
      if (I == 3)
      {
        static size_t periodic_counter = 0;
        
        if (periodic_counter++ == 4)
          action.Cancel ();
      }
      
      break;
    default:
      throw xtl::format_exception<xtl::bad_operation> ("action_handler", "Exception occured during executing of action %d", I);
  }
}

template <int I> void callback_handler ()
{
  printf ("[%08u] callback %d\n", get_clamped_time (), I);
}

void log_message (const char* stream, const char* message)
{
  printf ("[%08u] %s: %s\n", get_clamped_time (), stream, message);
}

int main ()
{
  printf ("Results of action_queue_result:\n");
  
//  LogFilter filter ("system.*", &log_message);
  
  ActionQueue::PushAction (&action_handler<1>, &callback_handler<1>, ActionThread_Background);
  ActionQueue::PushAction (&action_handler<2>, ActionThread_Background, 2.0);
  ActionQueue::PushAction (&action_handler<3>, ActionThread_Background, 2.0, 0.5);

  size_t end_time = common::milliseconds () + TEST_TIME;

  while (common::milliseconds () < end_time);

  return 0;
}
