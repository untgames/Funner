#include "shared.h"
#include <stdio.h>

using namespace math;

int main()
{
  matrix<float,3> M1(3);
  matrix_print("M1:",M1);
  matrix_print("M1=M1+5:",M1+=5);
  matrix_print("M1=M1-5:",M1-=5);
  matrix_print("M1=M1*5:",M1*=5);
  matrix_print("M1=M1/5:",M1/=5);
  matrix_print("M1:",M1);
  return 0;
}