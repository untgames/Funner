#include "shared.h"

int main ()
{
  printf ("Results of push_notifications_center_test:\n");
  
  try
  {
    PushNotificationsCenter center ("test");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
