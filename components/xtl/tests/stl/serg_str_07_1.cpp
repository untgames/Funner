//Тестирование rfind (string, pos)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of serg_str_07_1:\n");

  string s1 ("Hello, world.");
  string s2 ("world");
  string::size_type pos;

  printf ("s1: \"%s\"\n", s1.c_str ());
  printf ("s2: \"%s\"\n", s2.c_str ());

  pos = s1.rfind (s2);
  if ( pos != string::npos )
    printf ("Found at position: %u\n", pos);
  else
    printf ("Not found\n");

  pos = s1.rfind (s2, 6);
  if ( pos != string::npos )
    printf ("Found at position: %u\n", pos);
  else
    printf ("Not found\n");

  return 0;
}
