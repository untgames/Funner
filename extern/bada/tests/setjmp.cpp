#include <stdio.h>
#include <setjmp.h>

int main ()
{
  printf ("Results of setjmp_test:\n");
  
  jmp_buf j;
  
  int result = setjmp (j);
  
  if (result == 0)
  {
    printf ("first chance\n");
    longjmp (j, 3);
  }
  else
  {
    printf ("second chance: %d\n", result);
  }

  return 0;
}
