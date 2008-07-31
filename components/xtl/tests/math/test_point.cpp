#include<math_experimental/vector.h>
#include<math_experimental/plane.h>
#include"shared.h"

using namespace math;

int main()
{
  point<float> p1;
  point_print("p1=",p1);
  point<float> p2(1,2,3);
  point_print("p2=",p2);
  point<float> p3(p2);
  point_print("p3=",p3);

  point_print("\np1=",p1.move(1,5,9));
  vec_print  ("vector p1p2=",vec<float,3>(create_vector(p1,p2)));
  return 0;
}