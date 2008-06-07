#include <stdio.h>

#include <xtl/function.h>

#include <common/action_queue.h>
#include <common/exception.h>

using namespace common;

void action_handler (size_t action_id)
{
  switch (action_id)
  {
    case 0: printf ("action 0 executed.\n"); break;
    case 1: printf ("action 1 executed.\n"); break;
    case 2: raise <Exception> ("action_handler", "Exception occured during executing of action 2");
  }
}

void log_handler (const char* message)
{
  printf ("New log message: %s\n", message);
}

int main ()
{
  printf ("Results of action_queue_test:\n");
  
  ActionQueue action_queue;

  printf ("Action queue empty - %c\n", action_queue.IsEmpty () ? 'y' : 'n');
  printf ("Action queue has action 0 - %c\n", action_queue.HasAction (0) ? 'y' : 'n');
  printf ("Action queue has action 1 - %c\n", action_queue.HasAction (1) ? 'y' : 'n');

  action_queue.SetAction (0, 1, &action_handler);
  action_queue.SetAction (1, 2, 3, &action_handler);
  action_queue.SetAction (2, 1, &action_handler);

  printf ("Action queue empty - %c\n", action_queue.IsEmpty () ? 'y' : 'n');
  printf ("Action queue has action 0 - %c\n", action_queue.HasAction (0) ? 'y' : 'n');
  printf ("Action queue has action 1 - %c\n", action_queue.HasAction (1) ? 'y' : 'n');

  action_queue.DoActions (10, &log_handler);

  printf ("Action queue empty - %c\n", action_queue.IsEmpty () ? 'y' : 'n');
  printf ("Action queue has action 0 - %c\n", action_queue.HasAction (0) ? 'y' : 'n');
  printf ("Action queue has action 1 - %c\n", action_queue.HasAction (1) ? 'y' : 'n');

  action_queue.RemoveAction (1);

  printf ("Action queue has action 1 - %c\n", action_queue.HasAction (1) ? 'y' : 'n');

  action_queue.SetAction (0, 11, &action_handler);
  action_queue.SetAction (2, 15, &action_handler);

  action_queue.DoActions (20);

  action_queue.SetAction (0, 1, &action_handler);
  action_queue.SetAction (1, 2, 1, &action_handler);
  action_queue.SetAction (2, 5, &action_handler);

  printf ("Action queue empty - %c\n", action_queue.IsEmpty () ? 'y' : 'n');

  action_queue.Clear ();

  printf ("Action queue empty - %c\n", action_queue.IsEmpty () ? 'y' : 'n');
  
  return 0;
}
