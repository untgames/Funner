#include <common/file.h>

#include <stdio.h>
#include <string.h>

using namespace common;

const char* RESULTS_DIR_NAME = "test_dir";
const char* NESTED_DIR_NAME  = "test_dir/test_dir";

int main ()
{
  printf ("Results of remove_directory_test:\n");
  
  try
  {
    printf ("Is dir exists: %c\n", FileSystem::IsDir (RESULTS_DIR_NAME) ? 'y' : 'n');

    FileSystem::Mkdir (RESULTS_DIR_NAME);
    FileSystem::Mkdir (NESTED_DIR_NAME);

    printf ("Is dir exists: %c\n", FileSystem::IsDir (RESULTS_DIR_NAME) ? 'y' : 'n');

    FileSystem::Remove (RESULTS_DIR_NAME);

    printf ("Is dir exists: %c\n", FileSystem::IsDir (RESULTS_DIR_NAME) ? 'y' : 'n');
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
