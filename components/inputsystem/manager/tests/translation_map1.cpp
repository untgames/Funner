#include <stdio.h>

#include <xtl/function.h>

#include <input/translation_map.h>

using namespace input;

void my_event_handler (const char* event)
{
  printf ("New event: '%s'\n", event);
}

int main ()
{
  printf ("Results of translation_map1_test:\n");
  
  try
  {
    TranslationMap translation_map;

    size_t event1_id = translation_map.Add ("event1", "Replaced event1: event_name={0}, event_param1={1} end");
    translation_map.Add ("event1 *", "Replaced event1: event_name={0}, event_param1={1} end");

    translation_map.ProcessEvent ("event1");

    translation_map.SetHandler (&my_event_handler);

    translation_map.ProcessEvent ("event1");
    translation_map.ProcessEvent ("event1 12asd");

    translation_map.Remove (event1_id);

    printf ("Processing removed event...\n");
    translation_map.ProcessEvent ("event1");
    printf ("Removed event processed\n");

    translation_map.Add ("event1", "Replaced event1: event_name={0}, event_param1={1}end");  

    translation_map.ProcessEvent ("event1");

    translation_map.Clear ();

    printf ("Processing cleared event...\n");
    translation_map.ProcessEvent ("event1");
    printf ("Cleared event processed\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
