#include "shared.h"

int main ()
{
  printf ("Results of connection_test:\n");

  try
  {
    const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME);

    Client client (SERVER_NAME);

    printf ("description='%s'\n", client.Description ());
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
