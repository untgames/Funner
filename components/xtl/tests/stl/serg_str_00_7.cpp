//Тестирование конструктора (range)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test serg_str_00_7:\n");

  char A[] = {'H','e','l','l','o'};;
  string s1 (A, A+5);
  printf ("\"%s\"\n", s1.c_str ());

  return 0;
}
