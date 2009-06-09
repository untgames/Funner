#ifndef COMMONLIB_FILE_SYSTEM_TEST_HEADER
#define COMMONLIB_FILE_SYSTEM_TEST_HEADER

#include <climits>
#include <cstdio>
#include <cstring>

#include <stl/string>

#include <xtl/function.h>

#include <common/file.h>
#include <common/streams.h>
#include <common/strlib.h>

using namespace common;

/*
    Вывод основной информации о файле
*/

inline void PrintFileInfo (const char* file_name)
{
  FileHash file_hash;

  FileSystem::GetFileHash (file_name,file_hash);

  printf ("File '%s' info:\n",file_name);
  printf ("Size:             %lu\n",FileSystem::GetFileSize (file_name));
  printf ("CRC32 hash value: %08lx\n",file_hash.crc32);
  printf ("MD5 hash value:   {");

  for (size_t i=0;i<15;i++)
    printf ("%02x,",file_hash.md5 [i]);

  printf ("%02x}\n",file_hash.md5 [15]);
}

#endif
