#include <stdio.h>

#include <xtl/function.h>
#include <xtl/iterator.h>

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

    translation_map.Add ("event1",   "Replaced event1: event_name={0}, event_param1={1} end", "tag0");
    translation_map.Add ("event1 *", "Replaced event1: event_name={0}, event_param1={1} end", "event1");

    translation_map.ProcessEvent ("event1", &my_event_handler);
    translation_map.ProcessEvent ("event1 12asd", &my_event_handler);

    printf ("Translation map:\n");    
    
    size_t index = 0;

    for (TranslationMap::Iterator iter=translation_map.CreateIterator (); iter; ++iter, ++index)
      printf ("  item %u: event is '%s', replacement is '%s', tag is '%s'\n", index, iter->InputEvent (),
              iter->Replacement (), iter->Tag ());
              
    translation_map.Remove ("tag0");

    printf ("Processing removed event...\n");
    translation_map.ProcessEvent ("event1", &my_event_handler);
    printf ("Removed event processed\n");

    translation_map.Remove ("event1");

    printf ("Processing removed event...\n");
    translation_map.ProcessEvent ("event1 45", &my_event_handler);
    printf ("Removed event processed\n");

    translation_map.Add ("event1", "Replaced event1: event_name={0}, event_param1={1}end");  

    translation_map.ProcessEvent ("event1", &my_event_handler);

    translation_map.Clear ();

    printf ("Processing cleared event...\n");
    translation_map.ProcessEvent ("event1", &my_event_handler);
    printf ("Cleared event processed\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
