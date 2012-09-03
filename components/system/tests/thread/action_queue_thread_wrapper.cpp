#include "shared.h"

size_t get_clamped_time ()
{
  return common::milliseconds () / 1000 * 1000;
}

template <int I> void action_handler (Action& action)
{
  printf ("[%08u] action %d executed in thread %s\n", get_clamped_time (), I, Thread::GetCurrentThreadId () == action.CreatorThreadId () ? "creator" : "not creator");
  fflush (stdout);  

  if (I == 3)
    throw xtl::format_operation_exception ("action_handler", "Exception in action handler");
}

template <int I> void callback_handler (size_t thread_id)
{
  bool is_creator = Thread::GetCurrentThreadId () == thread_id;
  
  if (is_creator)
    Application::Exit (0);

  printf ("[%08u] callback %d executed in thread %s\n", get_clamped_time (), I, is_creator ? "creator" : "not creator");
  fflush (stdout);  
}

void log_message (const char* stream, const char* message)
{
  printf ("[%08u] %s: %s\n", get_clamped_time (), stream, message);
  fflush (stdout);
}

int main ()
{
  printf ("Results of action_queue_thread_wrapper_result:\n");
  
  LogFilter filter ("system.application", &log_message);
  
  ActionQueue::PushAction (&action_handler<1>, xtl::bind (&callback_handler<1>, Thread::GetCurrentThreadId ()), ActionThread_Background);
  ActionQueue::PushAction (&action_handler<2>, make_callback_wrapper (ActionThread_Current, xtl::bind (&callback_handler<2>, Thread::GetCurrentThreadId ())), ActionThread_Background);
  ActionQueue::PushAction (&action_handler<3>, ActionThread_Main);

  Application::Run ();

  return 0;
}
