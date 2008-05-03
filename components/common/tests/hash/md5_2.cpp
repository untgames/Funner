// Тест хеширования длинной строки 
//                      (c) Gred

#include <common/hash.h>
#include <string.h>
#include <stdio.h>

using namespace common;

int main ()
{
  printf ("Results of md5_2_test:\n");

  char source [] = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
  unsigned char digest [16];

  md5 (digest,source,strlen (source));

  printf ("hash = ");

  for (int i=0;i<16;i++)
    printf ("%02x",digest [i]);

  printf ("\n");

  return 0;
}
