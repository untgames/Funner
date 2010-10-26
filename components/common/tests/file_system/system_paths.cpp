#include "test.h"

int main ()
{
  printf ("Results of system_paths_test:\n");
  
  try
  {
    static const char* DIRS [] = {"/system/personal", "/system/profile", "/system/appdata", "/system/inetcache",
      "/system/baddir", "/my/link", "/system/temp"};

    FileSystem::MountLink ("/my/link", "/system/personal");

    for (size_t i=0; i<sizeof (DIRS)/sizeof (*DIRS); i++)
    {
      printf ("IsFileExist(%s): %s\n", DIRS [i], FileSystem::IsFileExist (DIRS [i]) ? "true" : "false");
    }
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
