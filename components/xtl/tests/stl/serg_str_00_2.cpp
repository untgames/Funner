//Тестирование конструктора (string, offs, count)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test serg_str_00_2:\n");

  string s1 ("How Do You Do?");
  string s2 (s1, 7, 7);
  printf ("\"%s\"\n", s2.c_str ());
  
  return 0;
}
