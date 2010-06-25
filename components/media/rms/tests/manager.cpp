#include "shared.h"

int main ()
{
  printf ("Results of manager_test:\n");

  try
  {
    MyServer    custom_server1 (1), custom_server2 (2);
    ServerGroup server_group1 ("group1");
    
    server_group1.Attach (custom_server1); 

    {
      Binding binding;

      {
        ServerGroup server_group2 ("group2");
        
        Group group;
        
        group.Add ("resource1");
        group.Add ("resource2");
        
        printf ("Attach servers\n");
        
        server_group2.Attach (custom_server1);
        
        {
          ServerGroupAttachment attachment ("group2", custom_server2);          

          printf ("Create binding\n");

          binding = server_group2.CreateBinding (group);
          
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
