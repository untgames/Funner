#include "shared.h"

using namespace media;

void dump (FontBuilder& builder)
{
  printf ("  source            = '%s'\n", builder.Source ());
  printf ("  name              = '%s'\n", builder.Name ());
  printf ("  family name       = '%s'\n", builder.FamilyName ());
  printf ("  style name        = '%s'\n", builder.StyleName ());
  printf ("  glyphs count      = %lu\n", builder.GlyphsCount ());
  printf ("  first glyph code  = %lu\n", builder.FirstGlyphCode ());
  printf ("  font size         = %lu\n", builder.FontSize ());
  printf ("  has kerning 0 - 1 = %d\n", builder.HasKerning (0, 1));
  printf ("  has kerning 1 - 0 = %d\n", builder.HasKerning (1, 0));

  printf ("  kerning 0 - 1     = %.2f %.2f\n", builder.Kerning (0, 1).x_kerning, builder.Kerning (0, 1).y_kerning);

  printf ("  font:\n");

  media::Font font = builder.Font ();

  printf ("    source            = '%s'\n", font.Source ());
  printf ("    name              = '%s'\n", font.Name ());
  printf ("    family name       = '%s'\n", font.FamilyName ());
  printf ("    style name        = '%s'\n", font.StyleName ());
  printf ("    glyphs count      = %lu\n", font.GlyphsCount ());
  printf ("    first glyph code  = %lu\n", font.FirstGlyphCode ());
  printf ("    font size         = %lu\n", font.FontSize ());
  printf ("    has kerning 0 - 1 = %d\n", font.HasKerning (0, 1));
  printf ("    has kerning 1 - 0 = %d\n", font.HasKerning (1, 0));

  printf ("    kerning 0 - 1     = %.2f %.2f\n", font.Kerning (0, 1).x_kerning, font.Kerning (0, 1).y_kerning);
}

int main ()
{
  printf ("Results of font_builder test:\n");

  try
  {
    FontBuilder builder;

    printf ("Empty builder:\n");
    dump (builder);

    builder.SetSource         ("source_name");
    builder.Rename            ("builder1");
    builder.SetFamilyName     ("family_name");
    builder.SetStyleName      ("style_name");
    builder.SetGlyphsCount    (2);
    builder.SetFirstGlyphCode (32);
    builder.SetFontSize       (10);

    KerningInfo kerning_info = { 2.f, 4.f};

    builder.InsertKerning (0, 1, kerning_info);

    printf ("Filled builder:\n");
    dump (builder);
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
