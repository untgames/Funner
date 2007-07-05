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
  printf ("Results of min2_test:\n");
  printf ("%s\n",min ((char*)"shoe",(char*)"shine",str_compare));
  
  return 0;
}
