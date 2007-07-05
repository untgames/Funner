#include <stl/hash_set>
#include <stdio.h>

using namespace stl;

/*
    Данный тест проверяет корректность работы пустой хэш-таблицы
*/

int main ()
{ 
  printf ("Results of hset1_test:\n");

  hash_set<int> s;

  s.insert (1);
  s.erase  (1);  
  s.erase  (0);
  s.erase  (0);  
  
  hash_set<int> s1 = s;
  
  s1 = s;
  
  s1.erase (3);
  s1.equal_range (4);
  s1.find (5);
  s1.count (6);

  printf ("successfull\n");

  return 0;
}
