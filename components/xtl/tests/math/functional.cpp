#include <shared.h>
#include <stdio.h>
using namespace math;
int main()
{
	component_fn<min_fn<int> > Fn;
	vec<int,4> V1(-2);
	vec<int,4> V2(5,0,0,0);
	vec<int,4> res;
	Fn(res,V1,V2);
	res=normalize(V2);
	vec_print("res=",res);
	if (V1==V2)
	{
		printf("They are equal");
	}
	return 0;
}