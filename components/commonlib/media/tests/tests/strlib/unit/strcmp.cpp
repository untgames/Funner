#include <common/strlib.h>
#include <stdio.h>

using namespace common;
using namespace stl;

struct CompareTest
{
  const char *s1, *s2;
  int        len;
};

CompareTest compare_tests [] = {
  {"","Hello",-1}, {"hello","hello",4}, {"abcd","abc",5},
  {"hello","world",0}, {"","",10}, {"hello","Hello",-1}, {"abcde","abcdf",5}
};

const size_t COMPARE_TESTS_NUM = sizeof (compare_tests)/sizeof (CompareTest);

int main ()
{
  printf ("Results of strcmp_test:\n");
  
  printf ("Compare tests:\n");

/*  for (size_t i=0;i<COMPARE_TESTS_NUM;i++)
  {    
    CompareTest& test = compare_tests [i];
    
    if (test.len == -1) printf ("strcmp('%s','%s')=%d\n",test.s1,test.s2,::strcmp (test.s1,test.s2));
    else                printf ("strncmp('%s','%s',%d)=%d\n",test.s1,test.s2,test.len,::strncmp (test.s1,test.s2,(size_t)test.len));
  }*/

  for (size_t i=0;i<COMPARE_TESTS_NUM;i++)
  {    
    CompareTest& test = compare_tests [i];
    
    if (test.len == -1) printf ("strcmp('%s','%s')=%d\n",test.s1,test.s2,common::strcmp (test.s1,test.s2));
    else                printf ("strncmp('%s','%s',%d)=%d\n",test.s1,test.s2,test.len,common::strncmp (test.s1,test.s2,(size_t)test.len));
  }
  
  printf ("Case compare tests:\n");  
  
/*  for (size_t i=0;i<COMPARE_TESTS_NUM;i++)
  {    
    CompareTest& test = compare_tests [i];
    
    if (test.len == -1) printf ("stricmp('%s','%s')=%d\n",test.s1,test.s2,::_stricmp (test.s1,test.s2));
    else                printf ("strnicmp('%s','%s',%d)=%d\n",test.s1,test.s2,test.len,::_strnicmp (test.s1,test.s2,(size_t)test.len));
  }*/

  for (size_t i=0;i<COMPARE_TESTS_NUM;i++)
  {    
    CompareTest& test = compare_tests [i];
    
    if (test.len == -1) printf ("stricmp('%s','%s')=%d\n",test.s1,test.s2,common::stricmp (test.s1,test.s2));
    else                printf ("strnicmp('%s','%s',%d)=%d\n",test.s1,test.s2,test.len,common::strnicmp (test.s1,test.s2,(size_t)test.len));
  }

  return 0;
}
