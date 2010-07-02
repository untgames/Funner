// Тестирование функции equal_range
#include <stdio.h>
#include <stl/hash_map>
#include "test.h"

using namespace stl;

int main () 
{
  typedef hash_multimap <int,int> MyHash;

  printf ("Results of hmap9m_test:\n");

  MyHash hm1;
  MyHash::const_iterator iter;

  hm1.insert_pair (3, 10);
  hm1.insert_pair (3, 20);
  hm1.insert_pair (3, 30);

  print ("hm1 =", hm1.begin (), hm1.end ());

  //Поиск всех элементов с ключом 3
  pair <MyHash::const_iterator,
        MyHash::const_iterator> p1;

  p1 = hm1.equal_range (3);

  printf ("Elements with key of 3:\n");
  for (iter=p1.first; iter!=p1.second; iter++)
     printf ("%d ", iter->second);
  printf ("\n");

  return 0;
}
