#include <clocale>
#include <cstdio>

#include <stl/string>

#include <common/log.h>
#include <common/utf_converter.h>

#include <media/font_converter.h>
#include <media/image.h>

using namespace media;
using namespace common;

const size_t FIRST_GLYPH_CODE = 32;
const size_t LAST_GLYPH_CODE  = 16384;
const size_t FONT_SIZE        = LAST_GLYPH_CODE - FIRST_GLYPH_CODE + 1;

void log_handler (const char* log, const char* message)
{
  printf ("'%s': '%s'\n", log, message);
}

int main (int argc, char *argv[])
{
  common::LogFilter log_filter ("*", log_handler);

  if (argc != 5)
  {
    printf ("Usage: ttf2xfont ttfname imagename xfontname glyph_size\n");
    return 1;
  }

  int glyph_size = atoi (argv [4]);

  if (glyph_size < 1)
  {
    printf ("Glyph size must be greater than 0\n");
    return 1;
  }

  try
  {
    Font     font;
    Image    image (1, 1, 1, PixelFormat_RGBA8);
    FontDesc font_desc;

    stl::wstring utf16_string (FONT_SIZE, 0);

    for (size_t i = FIRST_GLYPH_CODE; i <= LAST_GLYPH_CODE; i++)
      utf16_string[i - FIRST_GLYPH_CODE] = i;

    font_desc.file_name = argv [1];
    font_desc.char_codes_line = utf16_string.c_str ();
    font_desc.glyph_size = glyph_size;
    font_desc.first_glyph_code = FIRST_GLYPH_CODE;
    font_desc.glyph_interval = 5;

    convert (font_desc, font, image);

    image.Rename (argv [2]);
    
    font.Rename       (argv [3]);
    font.SetImageName (argv [2]);

    font.Save  (argv [3]);
    image.Save (argv [2]);
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
