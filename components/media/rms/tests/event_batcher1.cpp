#include "shared.h"

int main ()
{
  printf ("Results of event_batcher1_test:\n");

  try
  {   
    MyServer server;

    EventBatcher batcher;

    batcher.LoadResource ("A1");
    batcher.LoadResource ("A2");
    batcher.LoadResource ("A3");

    batcher.PrefetchResource ("B1");
    batcher.PrefetchResource ("B2");
    batcher.PrefetchResource ("B3");

    batcher.UnloadResource ("C1");
    batcher.UnloadResource ("C2");
    batcher.UnloadResource ("C3");

    batcher.LoadResource ("A1");
    batcher.LoadResource ("B1");
    batcher.LoadResource ("C1");

    batcher.PrefetchResource ("A2");
    batcher.PrefetchResource ("B2");
    batcher.PrefetchResource ("C2");

    batcher.UnloadResource ("A3");
    batcher.UnloadResource ("B3");
    batcher.UnloadResource ("C3");

    batcher.Flush (server);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
