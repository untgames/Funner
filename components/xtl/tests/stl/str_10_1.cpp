//Тестирование find_first_not_of (string, pos)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of str_10_1:\n");

  string s1 ("123-hello-123");
  string s2 ("1234567890-");
  string::size_type pos;

  printf ("s1: \"%s\"\n", s1.c_str ());
  printf ("s2: \"%s\"\n", s2.c_str ());

  pos = s1.find_first_not_of (s2);
  if ( pos != string::npos )
    printf ("Found at position: %lu\n", pos);
  else
    printf ("Not found\n");

  pos = s1.find_first_not_of (s2, pos+5);
  if ( pos != string::npos )
    printf ("Found at position: %lu\n", pos);
  else
    printf ("Not found\n");

  return 0;
}
