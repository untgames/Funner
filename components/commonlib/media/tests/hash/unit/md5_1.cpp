// Тест хеширования пустой строки 
//                      (c) Gred

#include <common/hash.h>
#include <stdio.h>
#include <string.h>

using namespace common;

int main ()
{
  printf ("Results of md5_1_test:\n");

  char          source [] = "";
  unsigned char digest [16];

  md5 (digest,source,strlen (source));

  printf ("hash = ");

  for (int i=0;i<16;i++)
    printf ("%02x",digest [i]);

  printf ("\n");

  return 0;
}
