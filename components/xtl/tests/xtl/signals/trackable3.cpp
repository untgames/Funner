#include "test.h"

void handler () {}

int main ()
{
  printf ("Results of trackable3_test:\n");
  
  connection c1;
  
  printf ("c1 before t1 create: %s\n", c1.connected () ? "connected" : "disconnected");
   
  {
    trackable t1;

    c1 = t1.connect_tracker (&handler);

    t1.connect_tracker (c1);
    
    printf ("c1 after t1 create: %s\n", c1.connected () ? "connected" : "disconnected");
  }
  
  printf ("c1 after t1 destroy: %s\n", c1.connected () ? "connected" : "disconnected");  

  return 0;
}
