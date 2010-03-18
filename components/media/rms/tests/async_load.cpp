#include "shared.h"

void my_progress_handler (Progress& progress)
{
  switch (progress.State ())
  {
    case ProgressState_Failed:
      printf ("%s: failed on processing '%s', %.1f%%\n", progress.Stage (), progress.Resource (), progress.Value () * 100.0f);
      break;
    case ProgressState_Processing:
      printf ("%s: processing '%s', %.1f%%\n", progress.Stage (), progress.Resource (), progress.Value () * 100.0f);
      break;
    case ProgressState_Processed:
      printf ("%s: processed, %.1f%%\n", progress.Stage (), progress.Value () * 100.0f);
      break;
    default:
      break;
  }
}

int main ()
{
  printf ("Results of async_load_test:\n");

  try
  {
    MyServer custom_server;
    ServerGroup server_group ("group1");

    server_group.Attach (custom_server);

    server_group.EnableCache ();

    printf ("Create groups\n");
    
    Group group1, group2, group3;
    
    group1.Add ("resource1");
    group1.Add ("resource2");
    group1.Add ("resource3");
    group1.Add ("resource4");
    
    printf ("Create binding for group1\n");
    
    Binding binding1 = server_group.CreateBinding (group1);
    
    printf ("Async load\n");
    
    binding1.AsyncLoad (&my_progress_handler);
        
    printf ("Wait\n");
    
    binding1.Load (); //for wait

    printf ("Exit\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
