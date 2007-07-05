//Тестирование find_last_not_of (char, pos)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of serg_str_11_4:\n");

  string s1 ("xxx-xxx");
  string::value_type c2 = 'x';
  string::size_type pos;

  printf ("s1: \"%s\"\n", s1.c_str ());

  pos = s1.find_last_not_of (c2);
  if ( pos != string::npos )
    printf ("'%c' found at position: %u\n", c2, pos);
  else
    printf ("'%c' not found\n", c2);

  pos = s1.find_last_not_of (c2, pos-1);
  if ( pos != string::npos )
    printf ("'%c' found at position: %u\n", c2, pos);
  else
    printf ("'%c' not found\n", c2);

  return 0;
}
