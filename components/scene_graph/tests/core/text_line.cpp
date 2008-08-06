#include "shared.h"

int main ()
{
  printf ("Results of text_line_test:\n");
  
  TextLine::Pointer text_line (TextLine::Create ());

  printf ("TextLine text = '%s'\n", text_line->Text ());
  printf ("TextLine font name = '%s'\n", text_line->Font ());

  text_line->SetText ("text");
  text_line->SetFont ("font");

  printf ("TextLine text = '%s'\n", text_line->Text ());
  printf ("TextLine font name = '%s'\n", text_line->Font ());

  return 0;
}
