//тестиоование функций сравнения
#include <stl/queue>
#include <stl/list>
#include <stdio.h>

using namespace stl;

int main () 
{
  printf ("Results of queue05_test:\n");

//  queue <int, list<int> > q1, q2;
  queue <int> q1, q2;

  q1.push (10);
  q1.push (20);
  q1.push (30);
  q2.push (30);
  q2.push (20);
  q2.push (10);

  printf ("q1 == q2 => %s\n", q1 == q2 ? "true" : "false");
  printf ("q1 != q2 => %s\n", q1 != q2 ? "true" : "false");
  printf ("q1 <  q2 => %s\n", q1 <  q2 ? "true" : "false");
  printf ("q1 >  q2 => %s\n", q1 >  q2 ? "true" : "false");
  printf ("q1 <= q2 => %s\n", q1 <= q2 ? "true" : "false");
  printf ("q1 >= q2 => %s\n", q1 >= q2 ? "true" : "false");

  return 0;
}
