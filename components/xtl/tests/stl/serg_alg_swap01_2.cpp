//Тестирование алгоритма iter_swap (iter1,iter2)
#include <stdio.h>
#include <stl/algorithm>
#include <stl/vector>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_swap01_2:\n");

  vector <int> v1;
  list <int> l1;

  for (int i=1; i<4; i++)
  {
     v1.push_back (i);
     l1.push_back (i+3);
  }

  printf ("Before exchanging first elements:\n");
  print ("v1 =", v1.begin (), v1.end ());
  print ("l1 =", l1.begin (), l1.end ());

  iter_swap (v1.begin (), l1.begin ());

  printf ("After exchanging first elements:\n");
  print ("v1 =", v1.begin (), v1.end ());
  print ("l1 =", l1.begin (), l1.end ());

  return 0;
}

