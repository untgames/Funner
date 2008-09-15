#include <cstdio>

#include <xtl/function.h>

#include <input/translation_map_registry.h>

using namespace input;

const char* TRANSLATION_MAP_REGISTRY_FILE_NAME = "data/translation_map_registry.xkeyreg";
const char* DST_FILE_NAME = "/io/stdout/test.xkeyreg";

int main ()
{
  printf ("Results of translation_map_registry2_test:\n");
  
  try
  {
    TranslationMapRegistry translation_map_registry (TRANSLATION_MAP_REGISTRY_FILE_NAME);

    translation_map_registry.Save (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
