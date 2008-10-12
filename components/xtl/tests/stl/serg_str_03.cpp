//Тестирование copy
#include <stdio.h>
#include <stl/string>
#include "test.h"

using namespace stl;

void test ()
{
  Char array1 [] = {'s','e','r','g','e','y',0};
  Char array2 [10] = {0};
  basic_string <Char> s1 (array1);
  s1.copy (array2, 10);
  basic_string <Char> s2 (array2);
  if (s1 == s2)
    printf ("s1 == s2\n");
  else
    printf ("s1 != s2\n");
}

int main()
{
  printf ("Results of serg_str_03:\n");

  string s1 ( "Hello World" );
  char array1 [20] = {0};
  char array2 [10] = {0};
  string::pointer     arr1_ptr = array1;
  string::value_type *arr2_ptr = array2;

  printf ("Original string: '%s'\n", s1.c_str ());

  string::size_type nArray1;
  nArray1 = s1.copy (arr1_ptr, 12);  // C4996
  printf ("The number of copied characters in array1 is: %lu\n", nArray1);
  printf ("The copied characters array1 is: '%s'\n", array1);

  string::size_type nArray2;
  nArray2 = s1.copy (arr2_ptr, 5, 6);  // C4996
  printf ("The number of copied characters in array2 is: %lu\n", nArray2);
  printf ("The copied characters array2 is: '%s'\n", arr2_ptr);

  test ();
  printf ("obj count=%d\n",Char::obj_count);

  return 0;
}
