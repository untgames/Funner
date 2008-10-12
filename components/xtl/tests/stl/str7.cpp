//Тестирование erase

#include <stdio.h>
#include <stl/string>

using namespace stl;

int main( )
{
  printf ("Result of test str7_test:\n");

  string s ="Hello World";
  basic_string <char>::iterator i;

  printf ("erase interval [%d..%lu] from '%s': ",3,s.size()-2,s.c_str ());
  i = s.erase (s.begin ()+3,s.end()-1);
  printf ("'%s' (first='%c')\n",s.c_str (),*i);

  s = "Hello World";
  printf ("erase %d character from '%s': ",5,s.c_str ());
  i = s.erase (s.begin ()+5);
  printf ("'%s' (first='%c')\n",s.c_str (),*i);

  s = "Hello computer";

  printf  ("erase [%d..%d] from '%s': ",6,13,s.c_str ());
  string s1 = s.erase (6,8);
  printf ("'%s'\n",s1.c_str ());

  printf  ("clear string '%s': ",s.c_str ());
  s.clear ();
  printf  ("'%s' (%s)\n",s.c_str (),s.empty ()?"empty":"not empty");

  return 0;
}
