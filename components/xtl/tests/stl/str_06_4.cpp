//Тестирование find (char, pos)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of str_06_4:\n");

  string s1 ("Hello, world.");
  char c2 = 'w';
  string::size_type pos;

  printf ("s1: \"%s\"\n", s1.c_str ());

  pos = s1.find (c2);
  if ( pos != string::npos )
    printf ("'%c' found at position: %lu\n", c2, pos);
  else
    printf ("'%c' not found\n", c2);

  pos = s1.find (c2, 8);
  if ( pos != string::npos )
    printf ("'%c' found at position: %lu\n", c2, pos);
  else
    printf ("'%c' not found\n", c2);

  return 0;
}
