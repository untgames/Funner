#include "shared.h"

int main ()
{
  printf ("Results of quat01_test:\n");
  
  quatf q1;
  
  dump ("default constructor", q1);
  
  quatf q2 (2.0f);
  
  dump ("scalar constructor", q2);
  
  quatf q3 (1.0f, 2.0f, 3.0f, 4.0f);
  
  dump ("component constructor", q3);
  
  dump ("copy constructor", quatf (q3));
  
  q3 = 5.0f;
  
  dump ("scalar assignment", q3);
  
  q3 = q2;
  
  dump ("quat assignment", q3);

  return 0;
}
