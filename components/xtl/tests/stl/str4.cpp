// Тестирование получения подстроки

#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test str4_test:\n");

  string s1 = "Heterological paradoxes";

  printf ("substr '%s'[%d..%d] = '%s'\n",s1.c_str (),6,12,s1.substr (6,7).c_str ());
  printf ("substr '%s'[default] = '%s'\n",s1.c_str (),s1.substr ().c_str ());  

  return 0;
}
