// Тестирование assign, operator =

#include <stdio.h>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Result of test str5_test:\n");

  string s, s1 = "Wide World ";
  const char *cs = "Out There";
  
  s = cs;  
  printf ("string <- c-string ('%s'):         s='%s'\n",cs,s.c_str ());
 
  s.assign (cs,3);  
  printf   ("string <- c-string ('%s'[0..%d]):   s='%s'\n",cs,2,s.c_str ());  

  s = s1;    
  printf   ("string <- string   ('%s'):       s='%s'\n",s1.c_str (),s.c_str ());

  s.assign (s1, 5, 5);
  printf   ("string <- string   ('%s'[%d..%d]): s='%s'\n",s1.c_str (),5,9,s.c_str ());

  s = '0';
  printf   ("string <- char     ('%c'):                 s='%s'\n",'0',s.c_str ());    
  
  s.assign (4,'!');  
  printf   ("string <- char     ('%c'x%d):               s='%s'\n",'!',4,s.c_str ());  

  s.assign (s1.begin ()+5,s1.end ()-1);
  printf   ("string <- interval ('%s'[%d..%d]): s='%s'\n",s1.c_str (),5,9,s.c_str ());   

  return 0;
}
