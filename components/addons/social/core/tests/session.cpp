#include "shared.h"

int main ()
{
  printf ("Results of session_test:\n");
  
  try
  {
    Session session ("test");

    printf ("Description    '%s'\n", session.Description ());
    printf ("IsUserLoggedIn %c\n",   session.IsUserLoggedIn () ? 'y' : 'n');

    session.LogIn (LoginCallback (), PropertyMap ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
