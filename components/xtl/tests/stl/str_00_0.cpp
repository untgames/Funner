//Тестирование конструктора ()
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test str_00_0:\n");

  string str;
  printf ("\"%s\"\n", str.c_str ());
  
  return 0;
}
