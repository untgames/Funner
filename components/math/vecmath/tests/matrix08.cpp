#include "shared.h"

int main ()
{
  printf ("Results of matrix08_test:\n");
  
  const float values1 [] = {1.0f,   3.0f,  5.0f,  7.0f,
                            11.0f, 13.0f, 17.0f, 19.0f,
                            23.0f, 29.0f, 31.0f, 37.0f,
                            41.0f, 43.0f, 49.0f, 53.0f};
  
  mat4f m = values1;

  dump ("transpose(m)", transpose (m));
  dump ("normalize(m)", normalize (m));
  dump ("inverse(m)", inverse (m));
  dump ("test inverse", equal (mat4f (1.0f), m * inverse (m), 0.1f));
  dump ("test transpose", equal (m, transpose (transpose (m)), 0.1f));
  
  return 0;
}
