#include <common/strlib.h>
#include <stdio.h>

using namespace common;

void test (const char* s,const char* pattern)
{
  printf ("'%s' %s '%s'\n",s,wcmatch (s,pattern)?"match":"not match",pattern);
}

void itest (const char* s,const char* pattern)
{
  printf ("'%s' %s '%s'\n",s,wcimatch (s,pattern)?"match":"not match",pattern);
}

int main ()
{
  printf ("Results of wildcard_test:\n");

  const char* s = "hello world";  
  
  const char* patterns [] = {
     "hello*","hello*?*","*world","hell*world*","*hello*?*world*???","*world?",
     "Hello World","HeLlO*","*WorlD?","*","?","hello wor",
     NULL
  };  
  
  printf ("case-sensetive:\n");
  
  for (const char** p=patterns;*p;p++)
    test (s,*p);
    
  test ("","*");
  test ("","?");
  test ("","");
  test ("",s);
  test (s,"");
    
  printf ("case-insensetive:\n");
  
  for (const char** p=patterns;*p;p++)
    itest (s,*p);    
    
  itest ("","*");
  test ("","?");
  itest ("","");
  itest ("",s);
  itest (s,"");    

  return 0;
}
