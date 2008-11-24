#include "shared.h"

int main ()
{
  printf ("Results of server_filters_test:\n");
  
  try
  {
    MyServer custom_server;
    Server   server (custom_server);      
           
    Group group;

    group.Add ("resource1.xmesh");
    group.Add ("resource2");
    group.Add ("resource3.bmp");
    group.Add ("resource4");
    
    server.SetFilters ("*.bmp");

    printf ("Create binding\n");
      
    Binding binding = server.CreateBinding (group);
        
    printf ("Load resources\n");

    binding.Load ();
    
    printf ("Add filters\n");
    
    server.AddFilters ("*.xmesh");    
    
    printf ("Recreate binding\n");
    
    binding = Binding ();
    binding = server.CreateBinding (group);

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
