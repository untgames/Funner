#include <math_experimental/vector.h>
#include <stdio.h>

using namespace math;

template <class T,size_t size> void vec_print(const char* Message,const vec<T,size>& V)
{
	printf("%s",Message);
	for (int i=0;i<size;i++)
	{
		printf("%.2f ",V[i]);
	}
	printf("\n");
}

int main()
{
  vec3f V1(4.0f);
  vec_print<float,3>("\nV1:   ",V1);
  vec3f V2(5.0f);                                                          
  vec_print<float,3>("\nV2:   ",V2);
  V1=V2;
  vec_print<float,3>("\nV1=V2:",V1);
//--------------------------------------------------------
  vec_print<float,3>("\nV1=5:      ",V1=5.0f);
  vec_print<float,3>("\nV2=V2+5:   ",V2+=5);
  vec_print<float,3>("\nV1=V1-2:   ",V1-=2);
  vec_print<float,3>("\nV2=V2*3:   ",V2*=3);
  vec_print<float,3>("\nV1=V1/2:   ",V1/=2);
//---------------------------------------------------------
  vec_print<float,3>("\nV1:   ",V1);
  vec_print<float,3>("\nV2:   ",V2);
//--------------------------------------------------------
  vec_print<float,3>("\nV2+5:   ",vec3f (V2+5));
  vec_print<float,3>("\nV1-2:   ",vec3f (V1-2));
  vec_print<float,3>("\nV2*3:   ",vec3f (V2*3));  
  vec_print<float,3>("\nV1/2:   ",vec3f (V1/2));
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
  vec_print<float,3>("\nV2+V1:   ",vec3f (V2+V1));
  vec_print<float,3>("\nV1-V2:   ",vec3f (V1-V2));
  vec_print<float,3>("\nV2*V1:   ",vec3f (V2*V1));
  vec_print<float,3>("\nV1/V2:   ",vec3f (V1/V2));
//---------------------------------------------------------
  vec_print<float,3>("\nV1:   ",V1);
  vec_print<float,3>("\nV2:   ",V2);
//------------------------------------------------------------
  vec_print<float,3>("\nNormalized V1: ",vec3f(normalize(V1)));
  vec_print<float,3>("\nAbsolute V2: ",vec3f(abs(V2)));
  vec_print<float,3>("\nMin of V1,V2: ",vec3f(min<float,3>(V1,V2)));
  vec_print<float,3>("\nV1*V2 (vectors): ",vec3f(V1^V2));
//------------------------------------------------------------
  printf("\nLength of V2= %.2f",V2.length());
  printf("\nV1*V2= %.2f",V1&V2);
  printf("\nAngle between V1,V2: %.2f",angle<float,3>(V1,V2));
//----------------------------------------------------------
  if (V1==V2) printf("\nV1=V2");

  if (V1.equal(V2,1000)) printf("\nV1=V2+(-)1000");
    else printf("\nV1!=V2");

  return 0;
}