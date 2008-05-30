#include <stdio.h>

#include <xtl/string.h>

using namespace xtl;

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

  for (size_t i=0; i<COMPARE_TESTS_NUM; i++)
  {    
    CompareTest& test = compare_tests [i];
    
    if (test.len == -1) printf ("xstrcmp('%s','%s')=%d\n", test.s1, test.s2, xstrcmp (test.s1, test.s2));
    else                printf ("xstrncmp('%s','%s',%d)=%d\n", test.s1, test.s2, test.len, xstrncmp (test.s1, test.s2, (size_t)test.len));
  }

  for (size_t i=0; i<COMPARE_TESTS_NUM; i++)
  {    
    CompareTest& test = compare_tests [i];
    
    if (test.len == -1) printf ("xstrcmp('%s','%s')=%d\n", test.s1, test.s2, xstrcmp (test.s1, test.s2));
    else                printf ("xstrncmp('%s','%s',%d)=%d\n", test.s1, test.s2, test.len, xstrncmp (test.s1, test.s2, (size_t)test.len));
  }
  
  printf ("Case compare tests:\n");  
  
  for (size_t i=0;i<COMPARE_TESTS_NUM;i++)
  {    
    CompareTest& test = compare_tests [i];
    
    if (test.len == -1) printf ("xstricmp('%s','%s')=%d\n", test.s1, test.s2, xstricmp (test.s1,test.s2));
    else                printf ("xstrnicmp('%s','%s',%d)=%d\n", test.s1, test.s2, test.len, xstrnicmp (test.s1, test.s2, (size_t)test.len));
  }

  for (size_t i=0;i<COMPARE_TESTS_NUM;i++)
  {    
    CompareTest& test = compare_tests [i];
    
    if (test.len == -1) printf ("xstricmp('%s','%s')=%d\n", test.s1, test.s2, xstricmp (test.s1, test.s2));
    else                printf ("xstrnicmp('%s','%s',%d)=%d\n", test.s1, test.s2, test.len, xstrnicmp (test.s1, test.s2, (size_t)test.len));
  }

  return 0;
}
