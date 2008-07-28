#include <math_experimental/quat.h>
#include "shared.h"
#include <stdio.h>
#include <math.h>

using namespace math;

int main()
{
  quat<float> Q1(3);
  quat_print("Q1:",Q1);
  printf("Q1 length= %f^0,5=%f\n",Q1.squared_length(),Q1.length());
  quat<float> Q2(4);
  quat_print("Q2:",Q2);
  printf("Q2 length= %f^0,5=%f\n",Q2.squared_length(),Q2.length());
  quat<float> Q3(Q1*Q2);
  quat_print("Q3:",Q3);
  printf("Q3 length= %f^0,5=%f\n",Q3.squared_length(),Q3.length());  
  quat_print("Normalized Q3: ",quat<float>( normalize(Q3)));
  return 0;
}