#include "test.h"

typedef signal<void ()> my_signal;

stl::auto_ptr<my_signal> self_destroy_signal;

void self_disconnect ()
{
  self_destroy_signal = 0;
}

int main ()
{
  printf ("Results of slot_self_deletion2:\n");
  
  self_destroy_signal = new my_signal;

  self_destroy_signal->connect (&self_disconnect);

  (*self_destroy_signal)();

  printf ("executed successfull\n");

  return 0;
}
