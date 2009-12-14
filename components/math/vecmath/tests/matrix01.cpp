#include "shared.h"

int main ()
{
  printf ("Results of matrix01_test:\n");
  
  mat4f m1;
  
  dump ("default constructor", m1);
  
  mat4f m2 = 2.0f;
  
  dump ("scalar constructor", m2);
  
  const float values [] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
  
  mat4f m3 = values;
  
  dump ("array constructor", m3);
  
  m3 = 4.0f;
  
  dump ("scalar assign", m3);

  return 0;
}
