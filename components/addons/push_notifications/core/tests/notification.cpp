#include "shared.h"

int main ()
{
  printf ("Results of notification_test:\n");
  
  try
  {
    Notification notification;

    printf ("default notification\n");

    dump (notification);

    PropertyMap properties;

    properties.SetProperty ("String", "StringValue");
    properties.SetProperty ("IntValue", 10);

    notification.SetProperties (properties);

    printf ("filled notification\n");

    dump (notification);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
