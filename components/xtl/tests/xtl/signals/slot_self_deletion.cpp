#include "test.h"

const size_t NUM = 100000;

typedef signal<void (int)> my_signal;

connection self_destroy_connections [NUM];
my_signal  self_destroy_signals [NUM];

void self_disconnect (int index)
{
  self_destroy_connections [index].disconnect ();
}

int main ()
{
  printf ("Results of slot_self_deletion:\n");

  for (int i=0; i<NUM; i++)
    self_destroy_connections [i] = self_destroy_signals [i].connect (&self_disconnect);
    
    printf ("in\n");    

  for (int i=0; i<NUM; i++)
  {
    self_destroy_signals [i](i);
  }
  
    printf ("out\n");      

  return 0;
}
