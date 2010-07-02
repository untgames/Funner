// Тестирование функции insert
#include <stdio.h>
#include <stl/hash_map>
#include "test.h"

using namespace stl;

int main () 
{
  typedef hash_map<int,int> MyHash;

  printf ("Results of hmap11_test:\n");

  MyHash hm1, hm2;

  hm1.insert_pair (1, 10);
  hm1.insert_pair (2, 20);
  hm1.insert_pair (3, 30);
  hm1.insert_pair (4, 40);

  print ("hm1 =", hm1.begin (), hm1.end ());

  pair <MyHash::iterator, bool> pr;
  pr = hm1.insert_pair (1, 10);

  if (pr.second == true)
    printf ("Insert (1,10) successfull\n");
  else 
    printf ("Insert failed: key 1 exist! Value: %d\n", pr.first->second);

  print ("hm1 =", hm1.begin (), hm1.end ());
  
  hm2.insert_pair (10, 100);
  hm2.insert (++hm1.begin (), --hm1.end ());  // The templatized version inserting a range

  print ("hm2 =", hm2.begin (), hm2.end ());

  return 0;
}
