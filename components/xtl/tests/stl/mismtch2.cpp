#include <stl/algorithm>
#include <string.h>
#include <stdio.h>

using namespace stl;

bool str_equal (const char* a,const char* b)
{
  return strcmp (a,b) == 0 ? 1 : 0;
}

int main ()
{
  printf ("Results of mismtch2_test:\n");

  const unsigned size = 5;
  
  const char* n1 [size] = {"Brett","Graham","Jack","Mike","Todd"}, *n2 [size];

  copy (n1,n1+5,n2);
  
  pair<const char**,const char**> result = mismatch (n1,n1+size,n2,str_equal);
  
  if (result.first == n1+size && result.second == n2+size)
    printf ("n1 and n2 are the same\n");
  else
    printf ("mismatch at index: %d\n",result.first-n1);
    
  n2 [2] = "QED";
  
  result = mismatch (n1,n1+size,n2,str_equal);
  
  if (result.first == n2+size && result.second == n2+size)
    printf ("n1 and n2 are the same\n");
  else
    printf ("mismatch at index: %d\n",result.first-n1);
    
  return 0;
}
