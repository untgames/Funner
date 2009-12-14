#include "shared.h"

int main ()
{
  printf ("Results of quat07_test:\n");
  
  quatf q1 (1.0f, 2.0f, 3.0f, 4.0f), q2 (5.0f, 6.0f, 7.0f, 8.0f);

  dump ("q1", q1);
  dump ("q2", q2);
  dump ("inner (q1, q2)", inner (q1, q2));
  
  return 0;
}
