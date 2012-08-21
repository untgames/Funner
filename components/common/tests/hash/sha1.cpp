#include <common/hash.h>
#include <string.h>
#include <stdio.h>

using namespace common;

int main ()
{
  printf ("Results of sha1_test:\n");

  char source [] = "service_id=4method=user.getInfov=1.0secret";
  unsigned char digest [20];

  sha1 (source, strlen (source), digest);

  printf ("hash = ");

  for (size_t i=0;i<sizeof(digest);i++)
    printf ("%02x",digest [i]);

  printf ("\n");

  return 0;
}
