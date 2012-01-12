#include "shared.h"

int main ()
{
  printf ("Results of achievement_test:\n");
  
  try
  {
    Achievement achievement;

    printf ("default achievement\n");

    dump (achievement);

    achievement.SetId ("Id1");
    achievement.SetTitle ("Title");
    achievement.SetHidden (true);
    achievement.SetProgress (0.5);
    achievement.SetHandle ((const void*)1);

    PropertyMap properties;

    properties.SetProperty ("String", "StringValue");
    properties.SetProperty ("IntValue", 10);

    achievement.SetProperties (properties);

    printf ("filled achievement\n");

    dump (achievement);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
