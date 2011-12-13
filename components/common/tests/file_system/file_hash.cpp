#include <string.h>
#include "test.h"

const char* FILE_NAME = "data/test.txt";

void print_file_hash (const FileHash& hash)
{
  printf ("CRC32 hash value: %08lx\n", hash.crc32);
  printf ("MD5 hash value:   {");

  for (size_t i = 0; i < 15; i++)
    printf ("%02x,", hash.md5 [i]);

  printf ("%02x}\n", hash.md5 [15]);
}

int main ()
{
  printf ("Results of file_hash_test:\n");

  FileHash hash;

  printf ("Full file hash:");

  FileSystem::GetFileHash (FILE_NAME, hash);

  print_file_hash (hash);

  printf ("Part file hash:");

  FileSystem::GetFileHash (FILE_NAME, 4, hash);

  print_file_hash (hash);

  printf ("Full part file hash:");

  FileSystem::GetFileHash (FILE_NAME, 1024, hash);

  print_file_hash (hash);

  return 0;
}
