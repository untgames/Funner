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
/*matrix<float,4> RM1 (rotatef(M_PI/2,vec4f(0,1,0,0)));
  quat<float> Q4(matrix_to_quat(RM1));
  quat_print("Q4:",Q4);
  vec4f V3(0,0,1,0);
  vec4f V2(Q4*V3);
  vec_print("V2:",V2);
  printf("Q1&Q2= %.2f\n",Q1&Q2);*/
  
  return 0;
}