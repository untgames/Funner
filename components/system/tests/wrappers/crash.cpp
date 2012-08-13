#include <stdio.h>

int main ()
{ 
  printf ("Results of crash_test:\n");

  volatile int* p = 0;
  
  *p = 0;    

  return 0;
}
