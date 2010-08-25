#include "test.h"

int main ()
{
  printf ("Results of system_paths_test:\n");

  static const char* DIRS [] = {"/system/personal", "/system/cookies", "/system/profile", "/system/appdata", "/system/inetcache",
    "/system/baddir", "/my/link", "/system/temp"};

  FileSystem::Mount ("/my/link", "/system/personal");

  for (int i=0; i<sizeof (DIRS)/sizeof (*DIRS); i++)
  {
    printf ("IsFileExist(%s): %s\n", DIRS [i], FileSystem::IsFileExist (DIRS [i]) ? "true" : "false");
  }

  return 0;
}
