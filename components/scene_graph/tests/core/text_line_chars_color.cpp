#include "shared.h"

void print_chars_colors (TextLine::Pointer text_line)
{
  size_t text_length = text_line->TextLength ();

  if (!text_length)
    return;

  printf ("chars colors one by one:\n");

  for (size_t i = 0; i < text_length; i++)
  {
    printf ("  char %u: ", i);
    dump (text_line->CharColor (i));
    printf ("\n");
  }

  printf ("chars colors getted by one piece:\n");

  xtl::uninitialized_storage <math::vec4f> chars_colors (text_length);

  text_line->CharsColors (0, text_length, chars_colors.data ());

  for (size_t i = 0; i < text_length; i++)
  {
    printf ("  char %u: ", i);
    dump (chars_colors.data () [i]);
    printf ("\n");
  }

  printf ("chars color factors one by one:\n");

  for (size_t i = 0; i < text_length; i++)
  {
    printf ("  char %u: ", i);
    dump (text_line->CharColorFactor (i));
    printf ("\n");
  }

  printf ("chars color factors getted by one piece:\n");

  xtl::uninitialized_storage <math::vec4f> chars_color_factors (text_length);

  text_line->CharsColorFactors (0, text_length, chars_color_factors.data ());

  for (size_t i = 0; i < text_length; i++)
  {
    printf ("  char %u: ", i);
    dump (chars_color_factors.data () [i]);
    printf ("\n");
  }
}

int main ()
{
  printf ("Results of text_line_chars_color_test:\n");

  media::FontLibrary font_library;

  TextLine::Pointer text_line (TextLine::Create (font_library));

  printf ("TextLine color = [%.2f %.2f %.2f %.2f]\n", text_line->Color ().x, text_line->Color ().y, text_line->Color ().z, text_line->Color ().w);

  text_line->SetColor (0.9f, 0.9f, 0.9f, 0.9f);
  text_line->SetTextUtf8  ("some text");

  printf ("TextLine color = [%.2f %.2f %.2f %.2f]\n", text_line->Color ().x, text_line->Color ().y, text_line->Color ().z, text_line->Color ().w);
  print_chars_colors (text_line);

  text_line->SetCharsColorFactors (2, 3, math::vec4f (1.f, 2.f, 0.1f, 0.f));

  printf ("TextLine color = [%.2f %.2f %.2f %.2f]\n", text_line->Color ().x, text_line->Color ().y, text_line->Color ().z, text_line->Color ().w);
  print_chars_colors (text_line);

  printf ("chars colors from 3 to 6:\n");

  xtl::uninitialized_storage <math::vec4f> chars_colors (4);

  text_line->CharsColors (3, 4, chars_colors.data ());

  for (size_t i = 0; i < 4; i++)
  {
    printf ("  char %u: ", i + 3);
    dump (chars_colors.data () [i]);
    printf ("\n");
  }
  
  stl::basic_string<unsigned int> result = toutf32 (L"Unicode text");

  text_line->SetTextUtf32 (result.c_str (), result.size ());  

  printf ("TextLine color = [%.2f %.2f %.2f %.2f]\n", text_line->Color ().x, text_line->Color ().y, text_line->Color ().z, text_line->Color ().w);
  print_chars_colors (text_line);

  return 0;
}
