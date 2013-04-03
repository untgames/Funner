#include <common/file.h>

int main ()
{
  printf ("Results of hello_world_test:\n");

  common::FileSystem::CopyFile ("data/test.txt", "/io/stdout");

  return 0;
}
