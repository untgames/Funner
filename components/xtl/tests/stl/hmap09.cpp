// Тестирование функции equal_range
#include <stdio.h>
#include <stl/hash_map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of hmap9_test:\n");

  hash_map <int, int> hm1;
  hash_map <int, int>::const_iterator iter;

  hm1.insert_pair (1, 10);
  hm1.insert_pair (2, 20);
  hm1.insert_pair (3, 30);

  //Поиск всех элементов с ключом 3
  pair <hash_map<int,int>::const_iterator,
        hash_map<int,int>::const_iterator> p1, p2;

  p1 = hm1.equal_range (3);

  for (iter=p1.first; iter!=p1.second; iter++)
     printf (" %d", iter->second);
  printf ("\n");

  print ("hm1 =", hm1.begin (), hm1.end ());

  p1 = hm1.equal_range (2);

  printf ("1st element with key >= 2: %d\n", p1.first->second);
  printf ("1st element with key > 2: %d\n", p1.second->second);

  p2 = hm1.equal_range (4);

  if ((p2.first  == hm1.end ()) && 
      (p2.second == hm1.end ()))
     printf ("Not Found!\n");
  else
     printf ("Found!\n");

  return 0;
}
