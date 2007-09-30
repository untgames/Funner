#include <stl/vector>
#include <stl/list>
#include <stl/algorithm>
#include <xtl/iterator.h>

#include "shared.h"

using namespace stl;

typedef xtl::iterator<int> my_iter;

void print (int x)
{
  printf ("%d ", x);
}

int main ()
{
  printf ("Results of iterator_test:\n");
  
  try
  {  
    static int array [] = {1, 2, 3, 4, 5};
    
    vector<int> vec (array, array + 5);
    list<int>   list (vec.begin (), vec.end ());  
    
    my_iter i1, i2 = make_iterator (list, 0), i3 = vec.begin (), i4 = vec.end ();
    
    TEST (!i1);  
    TEST (i2);
    TEST (i3);
    TEST (i4);
    
    printf ("dump range [iterator;iterator): ");
    
    for_each (i3, i4, &print);
    
    printf ("\n");
    
    printf ("dump [iterator;empty) range: ");
    
    for_each (i2, i1, &print);
    
    printf ("\n");
    
    printf ("reverse-dump [iterator;iterator) range: ");
    
    for_each (reverse_iterator<my_iter> (--my_iter (i2)), reverse_iterator<my_iter> (i2), &print);
    
    printf ("\n");
    
    i3.clear ();
    i4 = i2;    
    
    TEST (i2 != i1);
    TEST (--my_iter (i2) == i1);
    TEST (i3 == i1);
    TEST (i1 == i3);
    TEST (i4 == i2);

    *i1;    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
