#include "shared.h"

int main ()
{
  printf ("Results of matrix02_test:\n");
  
  const float values [] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
  
  mat4f m1 = values;
  
  dump ("m1.row (1)", m1.row (1));
  dump ("m1.column (1)", m1.column (1));

  return 0;
}
