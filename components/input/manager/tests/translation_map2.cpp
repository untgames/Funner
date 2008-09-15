#include <stdio.h>

#include <xtl/function.h>

#include <input/translation_map.h>

using namespace input;

const char* TRANSLATION_MAP_FILE_NAME = "data/translation_table.xkeymap";
const char* DST_FILE_NAME = "/io/stdout/test.xkeymap";

int main ()
{
  printf ("Results of translation_map2_test:\n");
  
  try
  {
    TranslationMap translation_map (TRANSLATION_MAP_FILE_NAME);

    translation_map.Save (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
