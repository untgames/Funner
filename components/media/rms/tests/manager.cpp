#include "shared.h"

int main ()
{
  printf ("Results of manager_test:\n");

  try
  {
    MyServer custom_server1 (1), custom_server2 (2);
    Server   server1 (custom_server1);

    {
      Binding binding;

      {
        ResourceManager manager;
        
        Group group;
        
        group.Add ("resource1");
        group.Add ("resource2");
        
        printf ("Attach servers");
        
        manager.Attach (server1);      
        
        {
          Server server2 (custom_server2);

          manager.Attach (server2);

          printf ("Create binding\n");

          binding = manager.CreateBinding (group);
          
          printf ("Load resources\n");
          
          binding.Load ();

          printf ("Delete server2\n");
        }      
        
        printf ("Delete resource manager\n");
      }

      printf ("Delete binding\n");
    }
    
    printf ("Delete server1\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
