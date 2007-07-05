// Тестирование операторов сравнения и функции compare

#include <stdio.h>
#include <stl/string>

using namespace stl;

const char* f (bool b)
{
  return b ? "true" : "false";
}

const char* cmp_res (int comp)
{
  return comp < 0 ? "<" : comp ? ">" : "==";
}

int main ()
{
  printf ("Result of test str3_test:\n");

  string s1 = "SGI STL", s2 = "MS STL";
  const char* cs;
  
  printf ("s1='%s'\ns2='%s'\n",s1.c_str (),s2.c_str ());
  printf ("s1 == s1: %s\n",f (s1 == s1));
  printf ("s1 != s2: %s\n",f (s1 != s2));
  printf ("s1 <= s2: %s\n",f (s1 <= s2));
  printf ("s1 >= s2: %s\n",f (s1 >= s2));
  printf ("s1 <  s2: %s\n",f (s1 <  s2));
  printf ("s1 >  s2: %s\n",f (s1 >  s2));

  s1 = "CAB";
  s2 = "CAB";
  
  printf ("'%s' %s '%s'\n",s1.c_str (),cmp_res (s1.compare (s2)),s2.c_str ()); 

  s1 = "AACAB";
  s2 = "CAB";

  printf ("'%s'[%d..%d] %s '%s'\n",s1.c_str (),2,4,cmp_res (s1.compare (2,3,s2)),s2.c_str ());
  printf ("'%s'[%d..%d] %s '%s'\n",s1.c_str (),0,2,cmp_res (s1.compare (0,3,s2)),s2.c_str ());

  s1 = "AACAB";
  s2 = "DCABD";
  
  printf ("'%s'[%d..%d] %s '%s'[%d..%d]\n",s1.c_str (),2,4,cmp_res (s1.compare (2,3,s2,1,3)),s2.c_str (),1,3);   

  s1 = "ABC";  
  cs = "DEF";

  printf ("'%s' %s '%s'\n",s1.c_str (),cmp_res (s1.compare (cs)),cs);

  s1 = "AACAB";  
  cs = "CAB";
  
  printf ("'%s'[%d..%d] %s '%s'\n",s1.c_str (),2,4,cmp_res (s1.compare (2,3,cs)),cs);    

  s1 = "AACAB";  
  cs = "ACAB";
  
  printf ("'%s'[%d..%d] %s '%s'[0..%d]\n",s1.c_str (),1,3,cmp_res (s1.compare (1,3,cs,3)),cs,2); 

  return 0;
}
