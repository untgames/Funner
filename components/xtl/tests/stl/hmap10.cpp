// Тестирование операторов сравнения
#include <stdio.h>
#include <stl/hash_map>
#include <stl/string>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of hmap10_test:\n");

  hash_map <int,string> hm1, hm2;
  string values [] = {"one", "two", "three", "four", "five"};
  int num_values = sizeof (values) / sizeof (string);

  for (int i=0; i<num_values; i++)
  {
    hm1.insert_pair (i, values[i]);
    hm2.insert_pair (i*i, values[i]);
  }

  print ("hm1 =", hm1.begin (), hm1.end ());
  print ("hm2 =", hm2.begin (), hm2.end ());

  //Сравнение производится как по ключам, так и по значениям
  printf ("hm1 == hm2 => %s\n", hm1 == hm2 ? "true" : "false");
  printf ("hm1 != hm2 => %s\n", hm1 != hm2 ? "true" : "false");
  printf ("hm1 <  hm2 => %s\n", hm1 <  hm2 ? "true" : "false");
  printf ("hm1 >  hm2 => %s\n", hm1 >  hm2 ? "true" : "false");
  printf ("hm1 <= hm2 => %s\n", hm1 <= hm2 ? "true" : "false");
  printf ("hm1 >= hm2 => %s\n", hm1 >= hm2 ? "true" : "false");

  return 0;
}
