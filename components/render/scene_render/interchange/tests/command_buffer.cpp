#include "shared.h"

int main ()
{
  printf ("Results of command_buffer_test:\n");

  CommandBuffer buffer;
  
  printf ("use count: %u\n", buffer.UseCount ());

  buffer.Resize (100);

  printf ("use count: %u\n", buffer.UseCount ());  

  return 0;
}
