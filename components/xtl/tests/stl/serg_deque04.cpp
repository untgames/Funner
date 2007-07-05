//тестирование функций вставки
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_deque04_test:\n");

  deque <int> c1, c2;
  deque <int>::iterator iter;
  
  c1.push_back (10);
  c1.push_back (20);
  c1.push_back (30);
  c2.push_back (40);
  c2.push_back (50);
  c2.push_back (60);

  print ("c1 = ", c1.begin(), c1.end());

  c1.insert ( ++c1.begin(), 100);
  print ("c1 = ", c1.begin(), c1.end());

  iter = ++c1.begin ();
  c1.insert (iter, 2, 200);
  print ("c1 = ", c1.begin(), c1.end());

  c1.insert (++c1.begin(), c2.begin(), --c2.end());
  print ("c1 = ", c1.begin(), c1.end());

  printf ("c1.size () = %u\n", c1.size ());

  return 0;
}

