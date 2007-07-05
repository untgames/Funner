// SerjTest: Тестирование swap

#include <stdio.h>
#include <stl/string>

using namespace stl;

int main () 
{
  printf ("Result of test str8_test:\n");
  
  string s1 = "First", s2 = "Second";
  
  printf  ("swap ('%s','%s'): ",s1.c_str (),s2.c_str ());
  s1.swap (s2);  
  printf  ("('%s','%s')\n",s1.c_str (),s2.c_str ());

  return 0;
}
