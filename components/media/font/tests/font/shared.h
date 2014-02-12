#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>

#include <common/log.h>

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

    printf ("      w = %.2f h = %.2f bx = %.2f by = %.2f ax = %.2f ay = %.2f\n", glyph.width, glyph.height, glyph.bearing_x, glyph.bearing_y, glyph.advance_x, glyph.advance_y);
  }

  printf ("    has kerning 0 - 1 = %d\n", font.HasKerning (0, 1));
  printf ("    has kerning 1 - 0 = %d\n", font.HasKerning (1, 0));

  printf ("    kerning 0 - 1     = %.2f %.2f\n", font.Kerning (0, 1).x_kerning, font.Kerning (0, 1).y_kerning);
}
