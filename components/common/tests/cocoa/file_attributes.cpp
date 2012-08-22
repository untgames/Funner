#include <stdio.h>

#include <xtl/string.h>
#include <xtl/uninitialized_storage.h>

#include <common/file.h>
#include <common/strlib.h>

using namespace common;

const char* ATTRIBUTE_NAME = "com.apple.MobileBackup";
const char* FILE_PATH      = "data";

int main ()
{
  printf ("Results of file_attributes_test:\n");
  
  try
  {
    printf ("Has attribute = '%c'\n", FileSystem::HasFileAttribute (FILE_PATH, ATTRIBUTE_NAME) ? 'y' : 'n');

    FileSystem::SetFileAttribute (FILE_PATH, ATTRIBUTE_NAME, 0, 0);

    printf ("Has attribute = '%c'\n", FileSystem::HasFileAttribute (FILE_PATH, ATTRIBUTE_NAME) ? 'y' : 'n');

    FileSystem::RemoveFileAttribute (FILE_PATH, ATTRIBUTE_NAME);

    printf ("Has attribute = '%c'\n", FileSystem::HasFileAttribute (FILE_PATH, ATTRIBUTE_NAME) ? 'y' : 'n');

    int val = 1;

    FileSystem::SetFileAttribute (FILE_PATH, ATTRIBUTE_NAME, &val, sizeof (val));

    printf ("Has attribute = '%c'\n", FileSystem::HasFileAttribute (FILE_PATH, ATTRIBUTE_NAME) ? 'y' : 'n');

    for (size_t i = 0; i < 10; i++)
    {
      FileSystem::SetFileAttribute (FILE_PATH, common::format ("%s_%d", ATTRIBUTE_NAME, i).c_str (), 0, 0);
    }

    for (size_t i = 0; i < 10; i++)
    {
      printf ("Has attribute = '%c'\n", FileSystem::HasFileAttribute (FILE_PATH, common::format ("%s_%d", ATTRIBUTE_NAME, i).c_str ()) ? 'y' : 'n');
    }
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }
  
  return 0;
}
