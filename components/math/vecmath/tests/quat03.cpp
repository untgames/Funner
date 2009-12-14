#include "shared.h"

int main ()
{
  printf ("Results of quat03_test:\n");
  
  quatf q1 (1.0f, 2.0f, 3.0f, 4.0f), q2 (5.0f, 6.0f, 7.0f, 8.0f);

  dump ("q1+q2", q1+q2);
  dump ("q1-q2", q1-q2);
  dump ("q1*q2", q1*q2);
  dump ("q1*2",  q1*2.0f);
  dump ("q1/2",  q1/2.0f);
  dump ("2*q1",  2.0f*q1);
  dump ("q1+=q2", quatf (q1)+=q2);
  dump ("q1-=q2", quatf (q1)-=q2);
  dump ("q1*=q2", quatf (q1)*=q2);
  dump ("q1*=2",  quatf (q1)*=2.0f);
  dump ("q1/=2",  quatf (q1)/=2.0f);

  return 0;
}
