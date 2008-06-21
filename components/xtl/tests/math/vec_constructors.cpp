#include <shared.h>
#include <stdio.h>


using namespace math;

//template <class T,size_t size> void vec_print(vec<T,size>& V);

int main()
{
	printf("Creating vectors with 5 elements:");
	//------------Конструктор по умолчанию--------------------
	printf("\nvec()       V =");
	vec<float,5> V;
	vec_print(V);
	//------------Конструктор с параметрами-------------------
	printf("\nvec(T& val) V1=");
	vec<int,5> V1(6);
	vec_print(V1);
	vec<int,5> V2(2);
	printf("\n            V2=");
	vec_print(V2);
	//-----------Конструкторы копирования----------------------
	vec<int,5> V3(V1*V2);
	printf("\nV3=V1*V2;   V3=");
	vec_print(V3);
	//---------------------------------------------------------
	vec<int,5> V4(V1/3);
	printf("\nV4=V1/3;    V4=");
	vec_print(V4);
	//---------------------------------------------------------
	//-----------Оптимизированные конструкторы-----------------
	printf("\nvec(vec& v1,v2,functional) V5=");
	vec<int,5> V5(V1,V2,component_fn<divides<int,int,int> > ());
	vec_print(V5);
	//---------------------------------------------------------
	printf("\nvec(vec& v1,val arg,func)  V6=");
	vec<int,5> V6(V1,int (2),component_fn<divides<int,int,int> > ());
	vec_print(V6);
	//---------------------------------------------------------
	//--------------С 4-мя параметрами-------------------------
	printf("\nvec(val,val,val,val)       V7=");
	vec<int,5> V7(1,3,5,6);
	vec_print(V7);
	//-------------Повышение размерности-----------------------
	printf("\nvec(vec<stze-1,type>,arg)  V8=");
	vec<int,6> V8(V7,12);
	vec_print(V8);
	return 0;
}


