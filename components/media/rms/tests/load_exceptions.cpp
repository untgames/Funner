#include "shared.h"

int main ()
{
  printf ("Results of load_exceptions_test:\n");
  
  try
  {
    MyServer    custom_server (1);
    ServerGroup server_group ("group1");
    
    server_group.Attach (custom_server);
    
    Group group;
    
    group.Add ("resource1");
    group.Add ("bad_resource");
    group.Add ("resource2");

    printf ("Create binding\n");

    Binding binding = server_group.CreateBinding (group);
    
    printf ("Prefetch resources\n");    
    
    binding.Prefetch ();
    
    printf ("Unload resources\n");    
    
    binding.Unload ();    

    printf ("Load resources\n");

    binding.Load ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
