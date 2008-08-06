#include "shared.h"

int main ()
{
  printf ("Results of text_line_test:\n");
  
  TextLine::Pointer text_line (TextLine::Create ());

  printf ("TextLine color = [%.2f %.2f %.2f %.2f]\n", text_line->Color ().x, text_line->Color ().y, text_line->Color ().z, text_line->Color ().w);
  printf ("TextLine text = '%s'\n", text_line->Text ());
  printf ("TextLine font name = '%s'\n", text_line->Font ());

  text_line->SetColor (0.1f, 0.2f, 0.3f, 0.4f);
  text_line->SetText  ("text");
  text_line->SetFont  ("font");

  printf ("TextLine color = [%.2f %.2f %.2f %.2f]\n", text_line->Color ().x, text_line->Color ().y, text_line->Color ().z, text_line->Color ().w);
  printf ("TextLine text = '%s'\n", text_line->Text ());
  printf ("TextLine font name = '%s'\n", text_line->Font ());

  return 0;
}
