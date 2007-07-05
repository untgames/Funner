//Тестирование конструктора (c-string, n)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test serg_str_00_3:\n");

  const char *cstr = "Hello, World!";
  string str (cstr, 5);
  printf ("\"%s\"\n", str.c_str ());
  
  return 0;
}
