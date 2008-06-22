#include "shared.h"
#include <stdio.h>

using namespace math;

int main()
{
  vec3f V1(4);
  vec_print<float,3>("\nV1:   ",V1);
  vec3f V2(5);
  vec_print<float,3>("\nV2:   ",V2);
//--------------------------------------------------------
  vec_print<float,3>("\nV2=V2+5:   ",V2+=5);
  vec_print<float,3>("\nV1=V1-2:   ",V1-=2);
  vec_print<float,3>("\nV2=V2*3:   ",V2*=3);
  vec_print<float,3>("\nV1=V1/2:   ",V1/=2);
//---------------------------------------------------------
  vec_print<float,3>("\nV1:   ",V1);
  vec_print<float,3>("\nV2:   ",V2);
//--------------------------------------------------------
  vec_print<float,3>("\nV2+5:   ",V2+5);
  vec_print<float,3>("\nV1-2:   ",V1-2);
  vec_print<float,3>("\nV2*3:   ",V2*3);  
  vec_print<float,3>("\nV1/2:   ",V1/2);
//---------------------------------------------------------
  vec_print<float,3>("\nV1:   ",V1);
  vec_print<float,3>("\nV2:   ",V2);
//---------------------------------------------------------
  vec_print<float,3>("\nV2=V2+V1:   ",V2+=V1);
  vec_print<float,3>("\nV1=V1-V2:   ",V1-=V2);
  vec_print<float,3>("\nV2=V2*V1:   ",V2*=V1);
  vec_print<float,3>("\nV1=V1/V2:   ",V1/=V2);
//---------------------------------------------------------
  vec_print<float,3>("\nV1:   ",V1);
  vec_print<float,3>("\nV2:   ",V2);
//--------------------------------------------------------
  vec_print<float,3>("\nV2+V1:   ",V2+V1);
  vec_print<float,3>("\nV1-V2:   ",V1-V2);
  vec_print<float,3>("\nV2*V1:   ",V2*V1);
  vec_print<float,3>("\nV1/V2:   ",V1/V2);
//---------------------------------------------------------
  vec_print<float,3>("\nV1:   ",V1);
  vec_print<float,3>("\nV2:   ",V2);
  return 0;
}