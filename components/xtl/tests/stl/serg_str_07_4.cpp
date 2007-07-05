//Тестирование rfind (char, pos)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of serg_str_07_4:\n");

  string s1 ("Hello, world.");
  string::value_type c2 = 'w';
  string::size_type pos;

  printf ("s1: \"%s\"\n", s1.c_str ());

  pos = s1.rfind (c2);
  if ( pos != string::npos )
    printf ("'%c' found at position: %u\n", c2, pos);
  else
    printf ("'%c' not found\n", c2);

  pos = s1.rfind (c2, 6);
  if ( pos != string::npos )
    printf ("'%c' found at position: %u\n", c2, pos);
  else
    printf ("'%c' not found\n", c2);

  return 0;
}
