#include <stl/algorithm>
#include <string.h>
#include <stdio.h>

using namespace stl;

static bool str_compare (const char* a, const char* b)
{
  return strcmp (a,b) < 0 ? 1 : 0;
}

int main ()
{
  printf ("Results of binsrch2_test:\n");

  const char* labels [] = {"aa","dd","ff","jj","ss","zz"};

  const unsigned count = sizeof(labels) / sizeof(labels[0]);
  
  if (binary_search (labels,labels+count,"ff",str_compare))
    printf ("ff is in labels.\n");
  else
    printf ("ff is not in labels.\n");
    
  return 0;
}
