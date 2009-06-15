#include "test.h"

typedef signal<void ()> my_signal;

my_signal* self_destroy_signal;

void self_disconnect ()
{
  self_destroy_signal->~my_signal ();
  
  self_destroy_signal = new (self_destroy_signal) my_signal;
  
  self_destroy_signal->connect (&self_disconnect);
}

int main ()
{
  printf ("Results of slot_self_deletion3:\n");
  
  self_destroy_signal = new my_signal;

  self_destroy_signal->connect (&self_disconnect);

  (*self_destroy_signal)();

  printf ("executed successfull (handlers count=%u)\n", self_destroy_signal->num_slots ());

  return 0;
}
