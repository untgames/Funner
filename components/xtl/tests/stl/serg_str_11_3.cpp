//Тестирование find_last_not_of (c-string, pos, count)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of serg_str_11_3:\n");

  string s1 ("xxx-yyy");
  const char* s2 = "xy";
  string::size_type pos;

  printf ("s1: \"%s\"\n", s1.c_str ());
  printf ("s2: \"%s\"\n", s2);

  pos = s1.find_last_not_of (s2, s1.size (), 2);
  if ( pos != string::npos )
    printf ("Found at position: %lu\n", pos);
  else
    printf ("Not found\n");

  pos = s1.find_last_not_of (s2, pos-1, 1);
  if ( pos != string::npos )
    printf ("Found at position: %lu\n", pos);
  else
    printf ("Not found\n");

  return 0;
}
