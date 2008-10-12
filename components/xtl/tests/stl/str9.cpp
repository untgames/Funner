// SerjTest: Тестирование copy

#include <stdio.h>
#include <stl/string>

using namespace stl;

int main( )
{
  printf ("Result of test str9_test:\n");

  string s = "Hello World";
  char buf [128];

  memset (buf,0,sizeof buf);
  printf ("copy %lu characters from '%s' to buf: '%s'\n",s.copy (buf,128),s.c_str (),buf);
  memset (buf,0,sizeof buf);
  printf ("copy %lu characters from '%s'[%d] to buf: '%s'\n",s.copy (buf,5,6),s.c_str (),6,buf);

  return 0;
}
