//Тестирование find_first_not_of
#include <stdio.h>
#include <stl/string>

using namespace stl;

int main()
{
  printf ("Results of serg_str_04:\n");

  // The first member function searches for a single character in a string
  string str1 ( "xddd-1234-abcd" );
  printf ("The original string str1 is: '%s'\n", str1.c_str ());
  string::size_type index;
  const string::size_type npos = string::npos;

  index = str1.find_first_not_of ("d", 2);
  if ( index != npos )
    printf ("found str[%d] = '%c'\n", index, str1 [index]);
  else
    printf ("The character 'd' was not found in str1.\n");

  index = str1.find_first_not_of ("x");
  if (index != npos )
    printf ("found str[%d] = '%c'\n", index, str1 [index]); 
  else
    printf ("The character 'non x' was not found in str1.\n");

  // The second member function searches a string for a substring as specified by a C-string
  string str2 ( "BBB-1111" );
  printf ("The original string str2 is: '%s'\n", str2.c_str ());

  const char *cstr2 = "B1";
  index = str2.find_first_not_of (cstr2, 6);
  if (index != npos)
    printf ("found str[%d] = '%c'\n", index, str2 [index]);
  else
    printf ("Elements of the substring 'B1' were not found in str2 after the 6th position.\n");

  index = str2.find_first_not_of (cstr2);
  if ( index != npos )
    printf ("found str[%d] = '%c'\n", index, str2 [index]);
  else
    printf ("The substring 'B1' was not found in str2.\n");

  string str3 ( "444-555-GGG" );
  printf ("The original string str3 is: '%s'\n", str3.c_str ());

  const char *cstr3 = "45G";
  string::size_type index2 = str3.find_first_not_of (cstr3);
  if ( index2 != npos )
    printf ("found str[%d] = '%c'\n", index2, str3 [index2]);
  else
    printf ("Elements in str3 contain only characters in the string '45G'.\n");

  index = str3.find_first_not_of (cstr3, index2 + 1, 2);
  if (index != npos)
    printf ("found str[%d] = '%c'\n", index, str3 [index]);
  else
    printf ("Elements in str3 contain only characters in the string '45G'.\n");

  // The fourth member function searches a string for a substring as specified by a string
  string str4 ( "12-ab-12-ab" );
  printf ("The original string str4 is: '%s'\n", str4.c_str ());

  string str4a ( "ba3" );
  index = str4.find_first_not_of (str4a, 5);
  if (index != npos)
    printf ("found str[%d] = '%c'\n", index, str4 [index]);
  else
   printf ("Elements other than those in the substring 'ba3' were not found in the string str4.\n");

  string str4b ( "12" );
  index = str4.find_first_not_of (str4b);
  if (index != npos )
    printf ("found str[%d] = '%c'\n", index, str4 [index]);
  else
    printf ("Elements other than those in the substring '12' were not found in the string str4.\n");
  
  return 0;
}
