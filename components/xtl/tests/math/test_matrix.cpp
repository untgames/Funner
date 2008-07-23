#include <math_experimental/matrix.h>
#include"shared.h"
#include<stdio.h>

using namespace math;
template<class T,size_t size>
matrix<T,size> create_matrix(const T& val)
{
  matrix<T,size> M;
  for(int i=0;i<size;i++)
  {
    for(int j=0;j<size;j++)
    {
      M[i][j]=val;
    }
  }
  return M;
}

int main()
{
  matrix<float,2> M1(2);
  matrix<float,2> M2;
  for (int i=0;i<2;i++)
  {
    for(int j=0;j<2;j++)
    {
      M2[i][j]=i+j;
    }
  }
  matrix_print("M1:",M1);
  matrix_print("M2:",M2);
  matrix_print("-M1:",matrix<float,2>(-M1));
  matrix_print("M1=M1+M2:",M1+=M2);
  matrix_print("M1=M1-M2:",M1-=M2);
  matrix_print("M1=M1*M2:",M1*=M2);
  matrix_print("M1=M1/M2:",M1/=M2);

  matrix_print("M1:",M1);
  matrix_print("M2:",M2);

  matrix_print("M1+M2:",matrix<float,2>(M1+M2));
  matrix_print("M1-M2:",matrix<float,2>(M1-M2));
  matrix_print("M1*M2:",matrix<float,2>(M1*M2));
  matrix_print("M2/M2:",matrix<float,2>(M2/M2));
  if (M1==M2)
  {
    printf("M1==M2");
  }
  else
  {
    printf("M1!=M2");
  }
  return 0;
}
