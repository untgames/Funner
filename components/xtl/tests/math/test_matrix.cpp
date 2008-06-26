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
  matrix<float,2> M1;
  for (int i=0;i<2;i++)
  {
    for(int j=0;j<2;j++)
    {
      M1[i][j]=i+j;
    }
  }
  matrix_print("M1:",M1);
  matrix<float,2> M2(2);
  matrix_print("M2:",M2);
/*  matrix_print("-M1:",matrix<float,5>(-M1));
  matrix_print("M1=M1+M2:",M1+=M2);
  matrix_print("M2=M2-M1:",M2-=M1);
  matrix_print("M1=M1*M2:",M1*=M2);
  matrix_print("M2=M2/M1:",M2/=M1);
  matrix_print("M1=M1*5:",M1*=5);

  matrix_print("M1:",M1);
  matrix_print("M2:",M2);
  matrix_print("M1+M2:",matrix<float,5>(M1+M2));
  matrix_print("M1-M2:",matrix<float,5>(M1-M2));*/
  matrix_print("M1*M2:",matrix<float,2>(M1*M2));
  matrix_print("three angle:",matrix<float,2>(three_angle_view(M1)));
//matrix_print("M1-1c:",matrix<float,1>(M1.delete_row_column(2,2)));
  matrix_print("normalized M1:",matrix<float,2>(normalize(M1)));
  printf("det= %f",det(M1));
//  matrix_print("M1/M2:",matrix<float,5>(M1/M2));
/*  matrix_print("M1+5:",matrix<float,5>(M1+5));
  matrix_print("M1-6:",matrix<float,5>(M1-6));*/

//  matrix_print("M16:",matrix<float,5>(M1-6));


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
