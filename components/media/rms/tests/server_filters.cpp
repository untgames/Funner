#include "shared.h"

int main ()
{
  printf ("Results of server_filters_test:\n");
  
  try
  {
    MyServer    custom_server;
    ServerGroup server_group ("group1");      
    
    server_group.Attach (custom_server);
           
    Group group;

    group.Add ("resource1.xmesh");
    group.Add ("resource2");
    group.Add ("resource3.bmp");
    group.Add ("resource4");
    
    server_group.SetFilters ("*.bmp");

    printf ("Create binding\n");
      
    Binding binding = server_group.CreateBinding (group);
        
    printf ("Load resources\n");

    binding.Load ();
    
    printf ("Add filters\n");
    
    server_group.AddFilters ("*.xmesh");    
    
    printf ("Recreate binding\n");
    
    binding = Binding ();
    binding = server_group.CreateBinding (group);

    printf ("Load resources\n");

    binding.Load ();    

    printf ("Destroy\n");    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
