//тестиоование конструкторов
#include <stdio.h>
#include <stl/vector>
#include <stl/list>
#include <stl/deque>
#include <stl/queue>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of queue01_test:\n");

  queue <Char> q1;                // Declares queue with default deque base container
  queue <Char, deque<Char> > q2;  // Explicitly declares a queue with deque base container

  // These lines don't cause an error, even though they
  // declares a queue with a vector base container
  queue <int, vector<int> > q3;
  q3.push (10);
  // but the following would cause an error because vector has 
  // no pop_front member function
  // q3.pop( );

  queue <int, list<int> > q4;

  list<int> li1;
  li1.push_back (1);
  li1.push_back (2);
  queue <int, list<int> > q5 (li1);
  printf ("The element at the front of queue q5 is %d.\n", q5.front ());
  printf ("The element at the back of queue q5 is %d.\n", q5.back ());

  return 0;
}

