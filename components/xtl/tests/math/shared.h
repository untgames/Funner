#include <math_experimental/forward.h>
#include <stdio.h>


using namespace math;

const float PI_=3.14159265358979323846f;

void _printf(int x);
void _printf(float x);
void _printf(const char* x);


template <class T,size_t size> void vec_print(const char* Message,const vec<T,size>& V)
{
	printf("%s",Message);
	for (int i=0;i<size;i++)
	{
		_printf(V[i]);
	}
	printf("\n");
}

template <class T,size_t size> void vec_print(const vec<T,size>& V)
{
	for (int i=0;i<size;i++)
	{
		if (V[i]>=0) printf(" ");
		_printf(V[i]);
	}

}

template <class T,size_t sizeX,size_t sizeY> void matrix_print(const char* Message,const matrix<T,sizeX,sizeY>& V)
{
	printf("%s",Message);
	for (int i=0;i<sizeX;i++)
	{
		printf("\n");
		vec_print(V[i]);
	}
	printf("\n");
}

template <class T> void quat_print(const char* Message,const quat<T>& V)
{
	printf("%s",Message);
	for (int i=0;i<4;i++)
	{
		_printf(V[i]);
	}
	printf("\n");
}


void _printf(int x)
{
	printf("%d ",x);
}

void _printf(float x)
{
	printf("%.2f ",x);
}

void _printf(const char* x)
{
	printf("%s ",x);
}
