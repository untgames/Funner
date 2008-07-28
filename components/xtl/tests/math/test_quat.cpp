#include <math_experimental/quat.h>
#include "shared.h"
#include <stdio.h>
#include <math.h>

using namespace math;

int main()
{
  //Конструкторы
  quat<float> Q1(3);
  quat_print("Q1: ",Q1);
  quat<float> Q2;
  quat_print("Q2: ",Q2);
  quat<float> Q3 (1,2,3,4);
  quat_print("Q3: ",Q3);

  //Унарные 
  quat_print("-Q1= ",-Q1);
  printf("Q1 length= %f^0,5=%f\n",Q1.squared_length(),Q1.length());

  //Бинарные
  quat_print("Q1=Q1+Q2= ",Q1+=Q2);
  quat_print("Q1=Q1-Q2= ",Q1-=Q2);
  quat_print("Q1=Q1*Q2= ",Q1*=Q2);

  quat_print("Q2+Q3= ",quat<float> (Q1+Q2));
  quat_print("Q2-Q3= ",quat<float> (Q1-Q2));
  quat_print("Q2*Q3= ",quat<float> (Q1*Q2));

  printf("Q1&Q2= %.2f",Q1&Q2);
    
  //Бинарные (кватернион - константа)
  quat_print("Q1=Q1*5= ",Q1*=5);
  quat_print("Q1=Q1/5= ",Q1/=5);

  quat_print("Q3*5= ",Q3*5);
  quat_print("Q3/5= ",Q3/5);

  //Сравнение
  if (Q1==Q2) printf("Q1==Q2\n");
  else printf("Q1!=Q2\n");
  if (Q2!=Q3) printf("Q2!=Q3\n");
  else printf("Q2==Q3\n");

  //Утилиты
  quat_print("Normalized Q3= ",quat<float>(normalize(Q3) ));
  quat_print("Inverted   Q3= ",quat<float>(normalize(Q3) ));

  float pitch,yaw,roll;
  quat_to_euler_angle (Q3,pitch,yaw,roll);
  printf("Q3= %.2f %.2f %.2f (in euler angle)",pitch,yaw,roll);

  return 0;
}