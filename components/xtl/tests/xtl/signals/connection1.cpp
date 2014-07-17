#include "test.h"

#include <stl/functional>

using namespace stl;

void handler ()
{
  printf ("Handler called\n");
}

int main ()
{
  printf ("Results of connection1_test:\n");

  try
  {
    xtl::signal<void ()> signal;

    {
      xtl::connection      connection1 = signal.connect (&handler);
      xtl::auto_connection connection2 = signal.connect (&handler);

      printf ("Call 1\n");
      signal ();
    }

    printf ("Call 2\n");

    signal ();

    {
      xtl::auto_connection connection2;

      {
        xtl::auto_connection connection1 = signal.connect (&handler);

        connection2 = connection1;

        xtl::connection ().swap (connection1);
      }

      {
        xtl::auto_connection connection1 = signal.connect (&handler);

        connection2 = connection1;

        xtl::connection ().swap (connection1);
      }
    }

    printf ("Call 3\n");

    signal ();

    printf ("Test end\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
