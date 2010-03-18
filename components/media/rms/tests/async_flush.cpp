#include "shared.h"

int main ()
{
  printf ("Results of async_flush_test:\n");

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
    
    group2.Add ("resource3");
    group2.Add ("resource5");
    
    group3.Add (group1);
    group3.Add (group2);
    group3.Remove ("resource2");
    group3.Add ("resource6");
    
    printf ("Create bindings\n");                
    
    Binding binding1 = server_group.CreateBinding (group1),
            binding2 = server_group.CreateBinding (group2),
            binding3 = server_group.CreateBinding (group3);            
            
    printf ("Async load\n");
            
    binding1.AsyncLoad ();
    binding2.AsyncLoad ();
    binding3.AsyncLoad ();
    
    printf ("Async unload\n");
    
    binding3.AsyncUnload ();
    
    printf ("Async flush unused resources\n");
    
    server_group.FlushUnusedResources ();
    
    printf ("Wait\n");
    
    ResourceManager::WaitForAsyncComplete ();
    
    printf ("Exit\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
