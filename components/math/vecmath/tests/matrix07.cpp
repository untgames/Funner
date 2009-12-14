#include "shared.h"

int main ()
{
  printf ("Results of matrix07_test:\n");
  
  const float values1 [] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
  const float values2 [] = {2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 1.0f};
  
  mat4f m1 = values1, m2 = values2;
  
  dump ("m1==m1", m1 == m1);
  dump ("m1!=m1", m1 != m1);
  dump ("m1==m2", m1 == m2);
  dump ("m1!=m2", m1 != m2);
  
  m2 = m1;
  
  m2 [3][3] += 0.09f;
  
  dump ("equal(m1,m2,0.1)", equal (m1, m2, 0.1f));
  dump ("equal(m1,m2,0.05)", equal (m1, m2, 0.05f));

  return 0;
}
