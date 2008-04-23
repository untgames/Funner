#include <stdio.h>

#include <xtl/function.h>

#include <input/action_map.h>

using namespace input;

void my_event_handler (const char* event_id, size_t arguments_count, const char* arguments [])
{
  printf ("New event: '%s' ", event_id);
  printf ("with %u args", arguments_count);
  if (arguments_count)
  {
    printf (":");

    for (size_t i = 0; i < arguments_count; i++)
      printf ("\n  %s", arguments[i]);
  }
  
  printf ("\n");
}

int main ()
{
  printf ("Results of action_map1_test:\n");
  
  try
  {
    ActionMap action_map;

    action_map.Add ("my_event", &my_event_handler);

    action_map.ProcessEvent ("my_event");
    action_map.ProcessEvent ("my_event 13");
    action_map.ProcessEvent ("");
    action_map.ProcessEvent ("badabum");

    action_map.Remove ("my_event");
    action_map.ProcessEvent ("my_event this_hadn't_be_appear");

    action_map.Add ("my_event", &my_event_handler);

    action_map.ProcessEvent ("my_event 1 2 3 4 5");

    action_map.Clear ();
    action_map.ProcessEvent ("my_event this_hadn't_be_appear");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
