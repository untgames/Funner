//Тестирование rfind (c-string, pos, count)
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of serg_str_07_3:\n");

  string s1 ("Hello, Serg.");
  const char* s2= "Sergey";
  string::size_type pos;

  printf ("s1: \"%s\"\n", s1.c_str ());
  printf ("s2: \"%s\"\n", s2);

  pos = s1.rfind (s2, s1.size (), 4);
  if ( pos != string::npos )
    printf ("Found at position: %u\n", pos);
  else
    printf ("Not found\n");

  pos = s1.rfind (s2, s1.size (), 6);
  if ( pos != string::npos )
    printf ("Found at position: %u\n", pos);
  else
    printf ("Not found\n");

  return 0;
}
