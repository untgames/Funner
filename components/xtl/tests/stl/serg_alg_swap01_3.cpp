//Тестирование алгоритма swap_ranges (first1, last1, first2)
#include <stdio.h>
#include <stl/algorithm>
#include <stl/vector>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_swap01_3:\n");

  vector <int> v1, v2;
  v1.push_back (1);
  v1.push_back (2);
  v2.push_back (3);
  v2.push_back (4);

  printf ("Before swap:\n");
  print ("v1 =", v1.begin (), v1.end ());
  print ("v2 =", v2.begin (), v2.end ());

  swap_ranges (v1.begin (), v1.end (), v2.begin ());

  printf ("After swap:\n");
  print ("v1 =", v1.begin (), v1.end ());
  print ("v2 =", v2.begin (), v2.end ());

  return 0;
}

