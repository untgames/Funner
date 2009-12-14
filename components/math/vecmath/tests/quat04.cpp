#include "shared.h"

int main ()
{
  printf ("Results of quat04_test:\n");
  
  quatf q1 (1.0f, 2.0f, 3.0f, 4.0f), q2 = q1;
  
  q2.w += 0.1f;
  
  dump ("q1==q2", q1==q2);
  dump ("q1!=q2", q1!=q2);
  dump ("q1==q1", q1==q1);
  dump ("q1!=q1", q1!=q1);
  dump ("equal(q1, q2, 0.1)", equal (q1, q2, 0.1f));
  dump ("equal(q1, q2, 0.05)", equal (q1, q2, 0.05f));

  return 0;
}
