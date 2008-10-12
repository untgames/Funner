//Тестирование изменения размера

#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test str2_test:\n");

  string s = "Hello world";

  printf   ("source:     string='%s', size=%lu\n",s.c_str (),s.size ());
  s.resize (s.size ()+3,'!');
  printf   ("1st resize: string='%s', size=%lu\n",s.c_str (),s.size ());
  s.resize (s.size ()+20);
  printf   ("2nd resize: string='%s', size=%lu\n",s.c_str (),s.size ());
  s.resize (s.size ()-28);
  printf   ("downsize:   string='%s', size=%lu\n",s.c_str (),s.size ());
  s.resize (0);
  printf   ("resize(0):  string='%s', size=%lu\n",s.c_str (),s.size ());
  printf   (s.empty ()?"String is empty\n":"String is not empty\n");

  return 0;
}
