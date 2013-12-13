#include <stdio.h>

#include <xtl/string.h>

using namespace xtl;

int main ()
{
  printf ("Results of sprintf_test:\n");
  
  char char_buffer [9];
  
  char_buffer [8] = 'A';
  
  printf ("result='%s', len=%d\n", char_buffer, xsnprintf (char_buffer, 8, "Hello %s","world"));
  printf ("control: '%c'\n", char_buffer [8]);
  printf ("len=%d\n", xsnprintf (0, 0, "Hello %s", "world"));

  return 0;
}
