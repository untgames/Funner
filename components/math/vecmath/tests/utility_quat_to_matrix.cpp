#include "shared.h"

int main ()
{
  printf ("Results of utility_quat_to_matrix_test:\n");
  
  quatf q = to_quat (degree (90.0f), vec3f (0, 0, 1));
  
  dump ("q", q);
  dump ("to_matrix (q)", to_matrix (q));
  
  mat3f m1;
  mat4f m2;
  
  to_matrix (q, m1);
  to_matrix (q, m2);
  
  dump ("to_matrix (q, m1)", m1);
  dump ("to_matrix (q, m2)", m2);

  return 0;
}
