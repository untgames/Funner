#include "shared.h"

int main ()
{
  printf ("Results of server_cache_test:\n");
  
  try
  {
    Binding  binding_that_destroyes_after_server;    
    
    {
      MyServer custom_server;
      Server   server (custom_server);
      
      server.EnableCache ();
      
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
      
      {
        printf ("Create binding for group1\n");
      
        Binding binding1 = server.CreateBinding (group1);
        
        printf ("Load resources for group1\n");

        binding1.Load ();        
        
        {      
          printf ("Create binding for group2\n");

          Binding binding2 = server.CreateBinding (group2);
          
          printf ("Load resources for group2\n");
          
          binding2.Load ();

          printf ("Delete binding for group2\n");
        }
        
        printf ("Disable cache\n");
      
        server.DisableCache ();        
        
        printf ("Delete binding for group1\n");
      }      
      
      printf ("Create binding for group3 = group1 + group2 - 'resource1'\n");
      
      binding_that_destroyes_after_server = server.CreateBinding (group3);
      
      printf ("Enable cache\n");
      
      server.EnableCache ();
      
      printf ("Prefetch resources for group3\n");      

      binding_that_destroyes_after_server.Prefetch ();      
      
      printf ("Recreate binding (check cache)\n");
      
      binding_that_destroyes_after_server = Binding ();
      binding_that_destroyes_after_server = server.CreateBinding (group3);      
      
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
