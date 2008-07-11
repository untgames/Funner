#include"shared.h"
#include <math_experimental/quat.h>
#include <math_experimental/vector.h>
#include <math_experimental/matrix.h>
#include <math_experimental/utils.h>
#include <stdio.h>

#include <math.h>
using namespace math;


int main()
{
  printf("180deg= %.2f rad\n",deg_to_rad<float> (float (180) ) );
  printf("PI rad= %.2f deg\n",rad_to_deg<float> (M_PI) );

  quat_print("quat from axis/angle",quat<float> (axis_angle_to_quat(float (180), vec<float,3>(0,1,0,0) ) ) );
  quat_print("quat from euler angle",quat<float> (euler_angle_to_quat(float (90),float(90),float(90) ) ) );

  matrix_print("Matrix translate (from vector (0,1,0) )", matrix_translate (vec<float,3>(0,1,0,0) ) );
  vec<float,3> V(0,1,0,0);
  matrix_print("Matrix rotate (from vector (0,1,0) )",
                (mat3f) (matrix3_rotate (float(90.0),V ) ) );
  matrix_print("Matrix scale (from vector (0,1,0) )",
                (mat3f) (matrix3_scale  (V) )  );

  return 0;

}
