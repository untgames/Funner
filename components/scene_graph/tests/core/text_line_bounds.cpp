#include "shared.h"

void dump (const aaboxf& box)
{
  printf ("[");
  dump (box.minimum ());
  printf (" - ");
  dump (box.maximum ());
  printf ("]\n");
}

int main ()
{
  printf ("Results of text_line_bounds_test:\n");

  media::FontLibrary font_library;

  font_library.LoadFont ("data/times.xfont");

  TextLine::Pointer text_line (TextLine::Create (font_library));

  text_line->SetTextUtf8 ("Text");
  text_line->SetFont     ("Times");

  media::FontCreationParams font_params;

  memset (&font_params, 0, sizeof (font_params));

  font_params.font_size      = 10;
  font_params.font_size_eps  = 1000;
  font_params.horizontal_dpi = 96;
  font_params.vertical_dpi   = 96;
  font_params.charset_name   = "";

  text_line->SetFontCreationParams (font_params);

  printf ("World bounds:\n");
  dump (text_line->WorldBoundBox ());
  printf ("Bounds:\n");
  dump (text_line->BoundBox ());
  printf ("World bounds:\n");
  dump (text_line->WorldBoundBox ());

  return 0;
}
