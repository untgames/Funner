//Тестирование конструктора (c-string)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test str_00_4:\n");

  const char *cstr = "Hello, World!";
  string str (cstr);
  printf ("\"%s\"\n", str.c_str ());
  
  return 0;
}
