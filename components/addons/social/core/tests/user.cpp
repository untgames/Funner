#include "shared.h"

int main ()
{
  printf ("Results of user_test:\n");
  
  try
  {
    User user;

    printf ("default user\n");

    dump (user);

    user.SetId ("Id1");
    user.SetNickname ("Nickname");
    user.SetFriend (true);
    user.SetHandle ((const void*)1);

    PropertyMap properties;

    properties.SetProperty ("String", "StringValue");
    properties.SetProperty ("IntValue", 10);

    user.SetProperties (properties);

    printf ("filled user\n");

    dump (user);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
