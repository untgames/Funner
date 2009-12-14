#include "shared.h"

int main ()
{
  printf ("Results of matrix04_test:\n");
  
  const float values [] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
  
  mat4f m1 = values, m2 = 2.0f;
  
  dump ("m1+m2", m1+m2);
  dump ("m1-m2", m1-m2);
  dump ("m1*m2", m1*m2);
  dump ("m1*2", m1 * 2.0f);
  dump ("2*m1", 2.0f* m1);
  dump ("m1 / 2", m1 / 2.0f);
  dump ("m1+=m2", mat4f (m1)+=m2);
  dump ("m1-=m2", mat4f (m1)-=m2);
  dump ("m1*=m2", mat4f (m1)*=m2);
  dump ("m1*=2", mat4f (m1)*=2.0f);
  dump ("m1/=2", mat4f (m1)/=2.0f);

  return 0;
}
