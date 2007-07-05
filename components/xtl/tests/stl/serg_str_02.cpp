//Тестирование replace
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of serg_str_02:\n");

  string result;
  string::iterator iter1, iter2;
  string s1o ( "AAAAAAAA" );
  string s1p ( "BBB" );
  const char* cs1p = "CCC";
  printf ("s1o:  '%s'\n", s1o.c_str ());
  printf ("s1p:  '%s'\n", s1p.c_str ());
  printf ("cs1p: '%s'\n", cs1p);
  result = s1o.replace (1, 3, s1p);
  printf ("The result of s1o.replace (1,3,s1p): '%s'\n", result.c_str ());
  result = s1o.replace (5, 3, cs1p);
  printf ("The result of s1o.replace (5,3,cs1p): '%s'\n", result.c_str ());

  s1o = "AAAAAAAA";
  result = s1o.replace (1, 3, s1p, 1, 2);
  printf ("The result of s1o.replace (1,3,s1p,1,2): '%s'\n", result.c_str ());
  result = s1o.replace (4, 3, cs1p, 1);
  printf ("The result of s1o.replace (4,3,cs1p,1): '%s'\n", result.c_str ());

  s1o = "AAAAAAAA";
  result = s1o.replace (1, 3, 4, 'C' );
  printf ("The result of s1o.replace (1,3,4,'C'): '%s'\n", result.c_str ());

  s1o = "AAAAAAAA";
  iter1 = s1o.begin ();
  iter2 = s1o.begin () + 3;
  result = s1o.replace (iter1, iter2, s1p);
  printf ("The result of s1o.replace (iter1,iter2,s1p): '%s'\n", result.c_str ());
  result = s1o.replace (iter1, iter2, cs1p);
  printf ("The result of s1o.replace (iter1,iter2,cs1p): '%s'\n", result.c_str ());

  s1o = "AAAAAAAA";
  iter1 = s1o.begin ();
  iter2 = s1o.begin () + 2;
  result = s1o.replace (iter1, iter2 , cs1p, 2);
  printf ("The result of s1o.replace (iter1,iter2,cs1p,2): '%s'\n", result.c_str ());

  s1o = "AAAAAAAA";
  iter1 = s1o.begin ();
  iter2 = s1o.begin () + 2;
  result = s1o.replace (iter1, iter2, 4, 'q');
  printf ("The result of s1o.replace (iter1,iter2,4,'q'): '%s'\n", result.c_str ());

  s1o = "AAAAAAAA";
  string::iterator iter3, iter4;
  iter1 = s1o.begin () + 1;
  iter2 = s1o.begin () + 3;
  iter3 = s1p.begin ();
  iter4 = s1p.begin () + 2;
  result = s1o.replace (iter1, iter2, iter3, iter4);
  printf ("The result of s1o.replace (iter1,iter2,iter3,iter4): '%s'\n", result.c_str ());

  return 0;
}
