#include<math_experimental/vector.h>
#include<math_experimental/plane.h>
#include<math_experimental/quat.h>
#include"shared.h"

using namespace math;

int main()
{
  plane<float,3> p1;
  plane_print("p1:",p1);
  plane<float,3> p2(vec3f(1,2,3),vec3f(0,3,5));
  plane_print("p2:",p2);
  plane<float,3> p3(plane_by_3points(vec3f(1,3,5),vec3f(2,4,6),vec3f(4,7,9)));
  plane_print("p3:",p3);
  plane<float,3> p4(plane_by_2crosslines(vec3f(1,4,5),vec3f(4,4,5),vec3f(2,4,6)));
  plane_print("p4:",p4);
  plane<float,3> p5(plane_by_2parallel_lines(vec3f(2,4,6),vec3f(1,6,7),vec3f(1,1,1)));
  plane_print("p5:",p5);
  plane<float,3> p6(p5);
  plane_print("p6:",p6);
  p6=p4;
  plane_print("p6=p4:",p6);
  quatf q(1);
  p6*=q;
  plane_print("rotatted p6:",p6);
  return 0;
}