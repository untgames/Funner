//Тестирование find_last_of (string, pos)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of str_09_1:\n");

  string s1 ("Hello, world.");
  string s2 (" ");
  string::size_type pos;

  printf ("s1: \"%s\"\n", s1.c_str ());
  printf ("s2: \"%s\"\n", s2.c_str ());

  pos = s1.find_last_of (s2);
  if ( pos != string::npos )
    printf ("Found at position: %lu\n", pos);
  else
    printf ("Not found\n");

  pos = s1.find_last_of (s2, 5);
  if ( pos != string::npos )
    printf ("Found at position: %lu\n", pos);
  else
    printf ("Not found\n");

  return 0;
}
