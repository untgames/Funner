//тестиоование функций доступа
#include <stdio.h>
#include <stl/vector>
#include <stl/queue>

using namespace stl;

int main () 
{
  printf ("Results of serg_pqueue03_test:\n");

  priority_queue <int> q1;
  priority_queue <int, vector<int>, greater<int> > q2;

  q1.push (20);
  q1.push (10);
  q1.push (30);

  q2.push (20);
  q2.push (10);
  q2.push (30);  

  printf ("length of q1: %u\n", q1.size ());

  printf ("top of q1: %d\n", q1.top ());
  printf ("top of q2: %d\n", q2.top ());

  return 0;
}

