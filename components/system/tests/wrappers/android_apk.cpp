#include "shared.h"

int main ()
{
  printf ("Results of android_apk_test:\n");

  try
  {
    printf ("File contents:\n%s\n", common::FileSystem::LoadTextFile ("META-INF/manifest.mf").c_str ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
