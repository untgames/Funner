//Тестирование конструктора (string)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test str_00_1:\n");

  string s1 ("How Do You Do?");
  string s2 (s1);
  printf ("\"%s\"\n", s2.c_str ());
  
  return 0;
}
