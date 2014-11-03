#include <common/file.h>
#include <common/strlib.h>

#include <stdio.h>
#include <string.h>

using namespace common;

const char* RESULTS_DIR_NAMES [] = { "test_dir", "test_dir/", "/mount/test_dir" };
const char* NESTED_DIR_NAMES []  = { "test_dir/test_dir", "test_dir/test_dir", "/mount/test_dir/test_dir" };

int main ()
{
  printf ("Results of remove_directory_test:\n");
  
  try
  {
    FileSystem::MountLink ("/mount", "/system/appdata");
    FileSystem::AddSearchPath ("/system/appdata");

    for (size_t i = 0, count = sizeof (RESULTS_DIR_NAMES) / sizeof (*RESULTS_DIR_NAMES); i < count; i++)
    {
      printf ("Test path '%s'...\n", RESULTS_DIR_NAMES [i]);

      printf ("Is dir exists: %c\n", FileSystem::IsDir (RESULTS_DIR_NAMES [i]) ? 'y' : 'n');

      FileSystem::Mkdir (RESULTS_DIR_NAMES [i]);
      FileSystem::Mkdir (NESTED_DIR_NAMES [i]);

      common::StdFile output_file (common::format ("%s/.hidden_file", NESTED_DIR_NAMES [i]).c_str (), common::FileMode_ReadWrite);

      output_file.Close ();

      printf ("Is dir exists: %c\n", FileSystem::IsDir (RESULTS_DIR_NAMES [i]) ? 'y' : 'n');

      FileSystem::Remove (RESULTS_DIR_NAMES [i]);

      printf ("Is dir exists: %c\n", FileSystem::IsDir (RESULTS_DIR_NAMES [i]) ? 'y' : 'n');
    }
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
