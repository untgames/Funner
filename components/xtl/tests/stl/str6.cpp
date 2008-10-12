//Тестирование append

#include <stdio.h>
#include <stl/string>

using namespace stl;

int main( )
{
  printf ("Result of test str6_test:\n");

  string s = "Hello ", s1 = "Wide World ", s2 = "Wide ", s3 = "World ";
  const char* cs = "Out There ";

  printf ("c-string:         '%s' + '%s' = '%s'\n",s.c_str (),cs,string (s).append (cs).c_str ());
  printf ("c-string:         '%s' + '%s'[0..%d] = '%s'\n",s.c_str (),cs,2,string (s).append (cs,3).c_str ());
  printf ("string:           '%s' + '%s'[%d..%d] = '%s'\n",s.c_str (),s1.c_str (),5,9,string (s).append (s1,5,5).c_str ());
  printf ("string, c-string: '%s' + '%s' + '%s' = '%s'\n",s.c_str (),s2.c_str (),s3.c_str (),((string (s)+=s2)+=s3).c_str ());
  printf ("char:             '%s' + '%c'x%d = '%s'\n",s.c_str (),'!',4,string (s).append (4,'!').c_str ());
  printf ("interval:         '%s' + '%s'[%d..%lu] = '%s'\n",s.c_str (),s1.c_str (),5,s1.size()-2,
          string (s).append (s1.begin()+5,s1.end ()-1).c_str ());

  return 0;
}
