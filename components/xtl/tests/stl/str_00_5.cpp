//Тестирование конструктора (n, char)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test str_00_5:\n");

  const char c = '9';
  string str (5, c);
  printf ("\"%s\"\n", str.c_str ());
  
  return 0;
}
