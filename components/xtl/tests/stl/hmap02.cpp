// Тестирование operator []
#include <stdio.h>
#include <stl/hash_map>
#include <stl/string>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of hmap2_test:\n");

  hash_map <string, int> hm1;

  hm1 ["one"] = 1;

  hm1.insert (hash_map <string, int>::value_type ("two", 2));
  hm1.insert_pair ("three", 3);

  // If the key already exists, operator[]
  // changes the value of the datum in the element
  hm1 ["two"] = 20;

  // operator[] will also insert the value of the data
  // type's default constructor if the value is unspecified
  hm1 ["five"];

  print ("hm1 =", hm1.begin (), hm1.end ());

  return 0;
}
