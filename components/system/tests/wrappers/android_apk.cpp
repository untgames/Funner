#include "shared.h"

int main ()
{
  printf ("Results of android_apk_test:\n");

  try
  {
    printf ("File contents:\n%s\n", common::FileSystem::LoadTextFile ("META-INF/manifest.mf").c_str ());

    common::FileSystem::AddSearchPath ("res");
    
    for (common::FileListIterator i = common::FileSystem::Search ("layout/*", common::FileSearch_Files | common::FileSearch_Sort); i; ++i)
      printf ("found '%s'\n", i->name);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
