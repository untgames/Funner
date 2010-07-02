// Тестирование конструкторов
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

void test ()
{
  list<Char> c1;
  c1.push_back ('a');
  c1.push_back ('b');
  c1.push_back ('c');

  list<Char> c2 (3, 'a');

  list<Char>::iterator iter = ++c1.begin ();
  list<Char> c3 (c1.begin (), ++iter);

  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());
  print ("c3 =", c3.begin (), c3.end ());
}

int main () 
{
  printf ("Results of list19_test:\n");

  test ();

  printf ("obj count=%d\n", Char::obj_count);

  return 0;
}
