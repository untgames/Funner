#include"shared.h"
#include<math_experimental/vector.h>
#include<math_experimental/quat.h>
#include<math_experimental/matrix.h>
#include<stdio.h>

using namespace math;

int main()
{
  quat<float> Q1(3);
  quat_print("Q1: ",Q1);

  vec4f v(1,1,1,1);
  vec_print("v=",v);

  float angle=PI_;

  //“β¨«¨βλ
  mat3f m(quat_to_matrix3(Q1));
  matrix_print("\nm (Q1 as matrix): ",m);

  quat_to_axis_angle  (Q1,angle,v);
  vec_print("\nQ1 as axis/angle:\n",v);
  printf("%.2f rad",angle);

  vec_print("\n\nQ1*v= ",vec4f (Q1*v));
  return 0;
}