#include "shared.h"

using namespace media;

const char* BAD_FONT_NAME = "data/bad_font.ttf";
const char* FONT_NAME     = "data/times_new_roman.ttf";

int main ()
{
  printf ("Results of ttf test:\n");

  try
  {
    FontLibrary library;

    library.LoadFont (FONT_NAME);

    printf ("Loaded fonts:\n");

    for (FontLibrary::ConstIterator iter = library.CreateIterator (); iter; ++iter)
    {
      printf ("  %s - %s\n", iter->FamilyName (), iter->StyleName ());
    }

    library.LoadFont (BAD_FONT_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
