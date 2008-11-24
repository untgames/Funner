#include "shared.h"

int main ()
{
  printf ("Results of server_no_cache_test:\n");
  
  try
  {
    Binding  binding_that_destroyes_after_server;    
    
    {
      MyServer custom_server;
      Server   server (custom_server);
      
      printf ("Create groups\n");
      
      Group group1, group2, group3;
      
      group1.Add ("resource1");
      group1.Add ("resource2");
      group1.Add ("resource3");
      
      group2.Add ("resource2");
      group2.Add ("resource4");
      group2.Add ("resource5");
      
      group3.Add (group1);
      group3.Add (group2);
      
      {
        printf ("Create binding for group1\n");
      
        Binding binding1 = server.CreateBinding (group1);
        
        printf ("Load resources for group1\n");

        binding1.Load ();        
        
        {      
          printf ("Create binding for group2\n");

          Binding binding2 = server.CreateBinding (group2);
          
          printf ("Prefetch resources for group2\n");
          
          binding2.Prefetch ();
          
          printf ("Unload resources for group2\n");
          
          binding2.Unload ();
          
          printf ("Delete binding for group2\n");
        }
        
        printf ("Delete binding for group1\n");
      }
      
      printf ("Create binding for group3 = group1 + group2\n");
      
      binding_that_destroyes_after_server = server.CreateBinding (group3);
      
      printf ("Load resources for group3\n");

      binding_that_destroyes_after_server.Load ();
      
      printf ("Remove server\n");
    }
    
    printf ("Remove binding for group3\n");    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
