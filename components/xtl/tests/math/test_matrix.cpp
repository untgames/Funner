#include"shared.h"
#include<stdio.h>

using namespace std;
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
  matrix<float,5,3> M1;
  matrix_print("M1:",M1);
  matrix<float,5,3> M2(3);
  matrix_print("M2:",M2);
  matrix_print("-M1:",matrix<float,5,3>(-M1));
  matrix_print("M1=M1+M2:",M1+=M2);
  matrix_print("M2=M2-M1:",M2-=M1);
  matrix_print("M1=M1*M2:",M1*=M2);
  matrix_print("M2=M2/M1:",M2/=M1);
  matrix_print("M1=M1*5:",M1*=5);

  matrix_print("M1:",M1);
  matrix_print("M2:",M2);
  matrix_print("M1+M2:",matrix<float,5,3>(M1+M2));
  matrix_print("M1-M2:",matrix<float,5,3>(M1-M2));
  matrix_print("M1*M2:",matrix<float,5,3>(M1*M2));
  matrix_print("M1/M2:",matrix<float,5,3>(M1/M2));
  matrix_print("M1+5:",matrix<float,5,3>(M1+5));
  matrix_print("M1-6:",matrix<float,5,3>(M1-6));

  if (M1==M2)
  {
    printf("M1=M2");
  }
  else
  {
    printf("M1!=M2");
  }
  return 0;
}
