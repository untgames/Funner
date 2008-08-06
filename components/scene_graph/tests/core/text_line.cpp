#include "shared.h"

const char* get_name (TextLineAlignment alignment)
{
  switch (alignment)
  {
    case TextLineAlignment_Center: return "TextLineAlignment_Center";
    case TextLineAlignment_Left:   return "TextLineAlignment_Left/TextLineAlignment_Top";
    case TextLineAlignment_Right:  return "TextLineAlignment_Right/TextLineAlignment_Bottom";
    default: return "Invalid";
  }
}

int main ()
{
  printf ("Results of text_line_test:\n");

  TextLine::Pointer text_line (TextLine::Create ());

  printf ("TextLine color = [%.2f %.2f %.2f %.2f]\n", text_line->Color ().x, text_line->Color ().y, text_line->Color ().z, text_line->Color ().w);
  printf ("TextLine text = '%s'\n", text_line->Text ());
  printf ("TextLine font name = '%s'\n", text_line->Font ());
  printf ("TextLine horizontal alignment = '%s', vertical_alignment = '%s'\n", get_name (text_line->HorizontalAlignment ()), get_name (text_line->VerticalAlignment ()));

  text_line->SetColor (0.1f, 0.2f, 0.3f, 0.4f);
  text_line->SetText  ("text");
  text_line->SetFont  ("font");
  text_line->SetAlignment (TextLineAlignment_Center, TextLineAlignment_Bottom);

  printf ("TextLine color = [%.2f %.2f %.2f %.2f]\n", text_line->Color ().x, text_line->Color ().y, text_line->Color ().z, text_line->Color ().w);
  printf ("TextLine text = '%s'\n", text_line->Text ());
  printf ("TextLine font name = '%s'\n", text_line->Font ());
  printf ("TextLine horizontal alignment = '%s', vertical_alignment = '%s'\n", get_name (text_line->HorizontalAlignment ()), get_name (text_line->VerticalAlignment ()));

  text_line->SetHorizontalAlignment (TextLineAlignment_Right);
  text_line->SetVerticalAlignment   (TextLineAlignment_Center);

  printf ("TextLine horizontal alignment = '%s', vertical_alignment = '%s'\n", get_name (text_line->HorizontalAlignment ()), get_name (text_line->VerticalAlignment ()));
  
  return 0;
}
