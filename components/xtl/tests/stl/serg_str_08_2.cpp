//Тестирование find_first_of (c-string, pos)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of serg_str_08_2:\n");

  string s1 ("Hello, world.");
  const char* s2 = " ";
  string::size_type pos;

  printf ("s1: \"%s\"\n", s1.c_str ());
  printf ("s2: \"%s\"\n", s2);

  pos = s1.find_first_of (s2);
  if ( pos != string::npos )
    printf ("Found at position: %u\n", pos);
  else
    printf ("Not found\n");

  pos = s1.find_first_of (s2, 7);
  if ( pos != string::npos )
    printf ("Found at position: %u\n", pos);
  else
    printf ("Not found\n");

  return 0;
}
