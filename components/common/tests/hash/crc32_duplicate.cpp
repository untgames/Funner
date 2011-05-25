#include <common/hash.h>
#include <cstdio>

using namespace common;

int main ()
{
  printf ("Results of crc32_duplicate_test:\n");

  size_t array1 [] = { 55351, 172781, 104441, 172781 }, array2 [] = { 18107, 54759, 34902, 54759 };

  printf ("Array1 hash = '%u'\n", crc32 (array1, sizeof (array1)));
  printf ("Array2 hash = '%u'\n", crc32 (array2, sizeof (array2)));

  return 0;
}
