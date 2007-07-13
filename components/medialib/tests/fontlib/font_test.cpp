#include <stdio.h>
#include <common/exception.h>
#include <media/font.h>

using namespace medialib;
using namespace common;

const char* file_name = "data/my_font.bfs";

int main ()
{
  try
  {
    FontFace* font2 = new FontFace (file_name);
    FontFace font1 (*font2);
    delete font2;

    printf ("Results of font_test:\n");
    printf ("Font '%s' original name = '%s';", file_name, font1.Name ());
    font1.Rename ("font_new_name");
    printf ("new name = '%s'\n", font1.Name ());
    printf ("Font '%s' image name = '%s'\n", font1.Name (), font1.ImageName ());
    printf ("glyphs count = %u, first glyph code = %u\n", font1.GlyphsCount (), font1.FirstGlyphCode ());
    printf ("Kerning between 0 and 1 = %d, between 1 and 0 = %d\n", font1.Kerning (0, 1)->x_kerning, font1.Kerning (1, 0)->x_kerning);
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ());
    return 1;
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
    return 1;
  }

  return 0;
}
