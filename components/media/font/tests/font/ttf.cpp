#include "shared.h"

using namespace media;

const char* BAD_FONT_NAME = "data/bad_font.ttf";
const char* FONT_NAME     = "data/times_new_roman.ttf";
//const char* FONT_NAME     = "data/vgafix.fon";

const char* CHARSET_NAME = "charset";

int main ()
{
  printf ("Results of ttf test:\n");

  try
  {
    CharsetManager::RegisterCharset (CHARSET_NAME, "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    FontLibrary library;

    library.LoadFont (FONT_NAME);

    printf ("Loaded fonts:\n");

    for (FontLibrary::ConstIterator iter = library.CreateIterator (); iter; ++iter)
    {
      printf ("  %s - %s\n", iter->FamilyName (), iter->StyleName ());

      FontCreationParams creation_params;

      memset (&creation_params, 0, sizeof (creation_params));

      creation_params.font_size      = 12;
      creation_params.font_size_eps  = 10;
      creation_params.weight         = FontWeight_Normal;
      creation_params.escapement     = 0;
      creation_params.bold           = false;
      creation_params.italic         = false;
      creation_params.underlined     = false;
      creation_params.striked        = false;
      creation_params.stroke_size    = 0;
      creation_params.horizontal_dpi = 72;
      creation_params.vertical_dpi   = 72;
      creation_params.charset_name   = CHARSET_NAME;

      Font font = library.CreateFont (iter->FamilyName (), creation_params);

      dump (font);
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
