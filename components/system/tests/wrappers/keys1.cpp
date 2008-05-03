#include "shared.h"

int main ()
{
  printf ("Results of keys1_test:\n");
  
  for (size_t i=0; i<Key_Num; i++)
    printf ("%s\n", get_key_name ((Key)i));

  return 0;
}
