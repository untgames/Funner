#include <stl/algorithm>
#include <string.h>
#include <stdio.h>

using namespace stl;

bool str_compare (const char* a,const char* b)
{
  return strcmp (a,b) < 0 ? 1 : 0;
}

int main ()
{
  printf ("Results of maxelem2_test:\n");

  const char* names [] = {"Brett","Graham","Jack","Mike","Todd"};

  const unsigned namesCt = sizeof (names)/sizeof (names[0]);
  
  printf ("%s\n",*max_element (names,names+namesCt,str_compare));
  
  return 0;
}
