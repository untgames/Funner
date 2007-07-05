//тестиоование конструкторов
#include <stdio.h>
#include <stl/vector>
#include <stl/deque>
#include <stl/queue>
#include <stl/algorithm>
#include "test.h"

template <class T> void unque (const char* title, T& q)
{
  printf ("%s = ( ", title);
  while (!q.empty ())
  {
     print (q.top ());
     printf (" ");
     q.pop ();
  }
  printf (")\n");
}

using namespace stl;

int main () 
{
  printf ("Results of serg_pqueue01_test:\n");

  priority_queue <int> q1;
  q1.push (100);
  q1.push (200);

  priority_queue <int, vector <int> > q2;
  q2.push (5);
  q2.push (15);
  q2.push (10);

  priority_queue <int, vector<int>, greater<int> > q3;
  q3.push (2);
  q3.push (1);
  q3.push (3);

  priority_queue <int> q4 (q1);

  int A[] = {5,5,5};
  priority_queue <int> q5 (A, A+3);

  priority_queue <int, vector<int>, greater<int> > q6 (A, A+3);

  unque ("q1", q1);
  unque ("q2", q2);
  unque ("q3", q3);
  unque ("q4", q4);
  unque ("q5", q5);
  unque ("q6", q6);
  printf ("After printing, q6 has %u elements.\n", q6.size ());

  return 0;
}
