//тестирование функции rbegin()/rend()
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of deque07_test:\n");

  deque <int> c1;
  deque <int>::reverse_iterator iter;
  
  for (int i=10; i<60; i+=10)
    c1.push_back (i);

  printf ("The deque contains the elements:\n");
  print ("c1 =", c1.begin (), c1.end ());

  printf ("The reversed deque is:\n");
  print ("c1 =", c1.rbegin (), c1.rend ());

  iter = c1.rbegin ();
  printf ("Last element in the deque is %d\n", *iter);
  iter = --c1.rend ();
  printf ("First element in the deque is %d\n", *iter);

  iter = c1.rbegin ();
  *iter = 40;
  printf ("Last element in deque is now %d\n", *iter);


  return 0;
}

