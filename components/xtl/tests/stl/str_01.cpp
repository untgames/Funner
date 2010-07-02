//Тестирование max_size и capacity
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test str_01:\n");

  string s (8, 'a');
  
  if (s.max_size () != 0)
    printf ("max_size () ok!\n");
  else
    printf ("max_size () bad!\n");

  if (s.capacity () != 8)
    printf ("capacity () ok!\n");
  else
    printf ("capacity () bad!\n");

  return 0;
}
