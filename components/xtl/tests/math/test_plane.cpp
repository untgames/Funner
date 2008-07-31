#include<math_experimental/vector.h>
#include<math_experimental/plane.h>
#include"shared.h"

using namespace math;

int main()
{
  plane<float> p1;
  plane_print("p1:",p1);
  plane<float> p2(vec3f(1,2,3),point<float>(0,3,5));
  plane_print("p2:",p2);
  plane<float> p3(point<float>(1,3,5),point<float>(2,4,6),point<float>(4,7,9));
  plane_print("p3:",p3);
  plane<float> p4(vec3f(1,4,5),vec3f(4,4,5),point<float>(2,4,6));
  plane_print("p4:",p4);
  plane<float> p5(point<float>(2,4,6),point<float>(1,6,7),vec3f(1,1,1));
  plane_print("p4:",p5);
  plane<float> p6(p5);
  plane_print("p6:",p6);


  
  return 0;
}