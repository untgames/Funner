#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>

#include <common/file.h>
#include <common/log.h>
#include <common/strlib.h>

#include <media/charset_manager.h>
#include <media/font_library.h>
#include <media/font.h>
#include <media/rasterized_font.h>

void dump (const media::Font& font)
{
  printf ("    source            = '%s'\n", font.Source ());
  printf ("    name              = '%s'\n", font.Name ());
  printf ("    family name       = '%s'\n", font.FamilyName ());
  printf ("    style name        = '%s'\n", font.StyleName ());
  printf ("    glyphs count      = %lu\n", font.GlyphsCount ());
  printf ("    first glyph code  = %lu\n", font.FirstGlyphCode ());
  printf ("    font size         = %lu\n", font.FontSize ());

  if (font.GlyphsCount ())
    printf ("    glyphs:\n");

  for (size_t i = 0, count = font.GlyphsCount (); i < count; i++)
  {
    const media::GlyphInfo& glyph = font.Glyphs () [i];

    printf ("      '%C' = w = %.2f h = %.2f bx = %.2f by = %.2f ax = %.2f ay = %.2f\n", font.FirstGlyphCode () + i, glyph.width, glyph.height, glyph.bearing_x, glyph.bearing_y, glyph.advance_x, glyph.advance_y);
  }

  printf ("    kernings:\n");

  for (size_t i = 0, count = font.GlyphsCount (); i < count ; i++)
  {
    for (size_t j = 0; j < count; j++)
    {
      if (font.HasKerning (i, j))
        printf ("      kerning '%C' - '%C' = %.2f %.2f\n", font.FirstGlyphCode () + i, font.FirstGlyphCode () + j, font.Kerning (i, j).x_kerning, font.Kerning (i, j).y_kerning);
    }
  }
}

void dump (const media::RasterizedFont& font, const media::Font& source_font)
{
  printf ("    images count = %lu\n", font.ImagesCount ());
  printf ("    rasterized glyphs:\n");

  for (size_t i = 0, glyphs_count = source_font.GlyphsCount (); i < glyphs_count; i++)
  {
    const media::RasterizedGlyphInfo& glyph = font.RasterizedGlyphs () [i];

    printf ("      '%C' = image = %lu x = %lu y = %lu w = %lu h = %lu\n", source_font.FirstGlyphCode () + i, glyph.image_index, glyph.x_pos, glyph.y_pos, glyph.width, glyph.height);
  }
}
