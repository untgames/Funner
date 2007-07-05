// Тестирование remove_if
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

template <class T> class is_odd : public stl::unary_function<T, bool> 
{
public:
   bool operator () (T& val) 
   {
     return ( val % 2 ) == 1;
   }
};

int main () 
{
  printf ("Results of serg_list13_test:\n");

  list<int> c1;
  
  c1.push_back (3);
  c1.push_back (4);
  c1.push_back (5);
  c1.push_back (6);
  c1.push_back (7);
  c1.push_back (8);

  print ("c1 =", c1.begin (), c1.end ());
  
  c1.remove_if (is_odd<int> ());

  printf ("After removing the odd elements, the list becomes:\n");
  print ("c1 =", c1.begin (), c1.end ());

  return 0;
}
