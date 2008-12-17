#include <cstdio>

#include <common/strlib.h>

int main ()
{
  printf ("Results of strerror_test:\n");
 
  const int MAX_ERROR_CODE = 42;
  
  for (int i=0; i<=MAX_ERROR_CODE; i++)
    printf ("%02d: '%s'\n", i, common::strerror (i));
  
  return 0;
}
