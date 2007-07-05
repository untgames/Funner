// Тестирование erase () и clear ()
#include <stdio.h>
#include <stl/hash_map>
#include <stl/string>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_hmap4_test:\n");

  hash_map <int, int> hm1, hm2, hm3;
  hash_map <int, int>::size_type n;
  int i;

  for (i=1; i<5; i++)
  {
    hm1.insert (make_pair (i, i));
    hm2.insert (make_pair (i, i*i));
    hm3.insert (make_pair (i, i-1));
  }

  print ("hm1 =", hm1.begin (), hm1.end ());
  print ("hm2 =", hm2.begin (), hm2.end ());
  print ("hm3 =", hm3.begin (), hm3.end ());

  hm1.erase (++hm1.begin ());

  printf ("Delete 2nd element from hm1:\n");
  print ("hm1 =", hm1.begin (), hm1.end ());

  hm2.erase (++hm2.begin(), --hm2.end());

  printf ("Delete [begin++, end--] from hm2:\n");
  print ("hm2 =", hm2.begin (), hm2.end ());

  n = hm3.erase (2);
  printf ("Delete elements with key of 2 from hm3:\n");
  print ("hm3 =", hm3.begin (), hm3.end ());

  printf ("Count of deleted elements: %d.\n", n);

  hm3.erase (++hm3.begin ());
  printf ("Delete 2nd element: \n");
  print ("hm3 =", hm3.begin (), hm3.end ());

  printf ("Size of hm3 before: %d\n", hm3.size ());
  hm3.clear ();
  printf ("Size of hm3 after: %d\n", hm3.size ());

  return 0;
}
