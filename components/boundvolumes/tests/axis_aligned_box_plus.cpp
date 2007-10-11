#include "shared.h"

int main ()
{
  printf ("Results of axis_aligned_box_plus_test:\n");  
  
  aaboxf box,box1 (vec3f (-10, -20, -30), vec3f (30, 20, 10)), box2 (1, 2, 3, 4, 5, 6);  
  spheref s(vec3f (20, 30, 40), 5);
  
  printf ("\noperator +=\n");
  box1+=s;
  dump (box1);
  printf ("\noperator +\n");
  box=box2+s;
  dump (box2);
  printf ("\ncontains must return true\n%d",contains(box1,s));
  printf ("\ncontains must return false\n%d",contains(box2,s));
  printf ("\n");
  
  
  return 0;
}
