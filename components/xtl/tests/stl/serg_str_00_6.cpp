//Тестирование конструктора (allocator)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test serg_str_00_6:\n");

  string s1;
  string s2 (s1.get_allocator ());
  if (s2.empty ())
    printf ("s2 is empty.\n");
  else
    printf ("s2 is not empty.\n");

  return 0;
}
