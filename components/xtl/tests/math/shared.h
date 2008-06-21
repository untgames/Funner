#include <math_experimental/mathlib.h>
#include <stdio.h>
#include <iostream.h>
using namespace math;

void _printf(int x);
void _printf(float x);
void _printf(const char* x);

template <class T,size_t size> void vec_print(const char* Message,vec<T,size>& V);

template <class T,size_t size> void vec_print(const char* Message,vec<T,size>& V)
{
	printf("%s",Message);
	for (int i=0;i<size;i++)
	{
		_printf(V[i]);
	}
	printf("\n");
}

template <class T,size_t size> void vec_print(vec<T,size>& V);

template <class T,size_t size> void vec_print(vec<T,size>& V)
{
	for (int i=0;i<size;i++)
	{
		_printf(V[i]);
	}
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
