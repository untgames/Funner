#include "test.h"

int main ()
{
  printf ("Results of file_name_pattern_test:\n");

  putenv ("TEST=<TEST>");

  const char* names [] = {"<TEST>/explorer.exe", "<TEST/explorer.exe", "<TEST!!!!!!>/explorer.exe"};
  size_t      names_count = sizeof (names) / sizeof (*names);

  for (size_t i=0; i<names_count; i++)
    printf ("'%s' -> '%s'\n", names [i], FileSystem::GetNormalizedFileName (names [i]).c_str ());

  return 0;
}
