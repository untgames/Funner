#include "test.h"

int main ()
{
  printf ("Results of file_system_info_test:\n");
  
  try
  {
    static const char* DIRS [] = {"/system/personal", "/system/profile", "/system/appdata", "/system/inetcache",
      "/system/baddir", "/my/link", "/system/temp", "data"};

    FileSystem::MountLink ("/my/link", "/system/personal");

    for (size_t i=0; i<sizeof (DIRS)/sizeof (*DIRS); i++)
    {
      filesize_t free_space  = FileSystem::GetFreeSpace (DIRS [i]),
                 total_space = FileSystem::GetTotalSpace (DIRS [i]);

      printf ("FileSystem(%s) info:\n", DIRS [i]);
      printf ("  free space - ");

      if (free_space == (filesize_t)-1)
        printf ("can't be determined\n");
      else
        printf ("%llu\n", free_space);

      printf ("  total space - ");

      if (total_space == (filesize_t)-1)
        printf ("can't be determined\n");
      else
        printf ("%llu\n", total_space);
    }
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
