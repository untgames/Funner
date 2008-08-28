#include "shared.h"

void print_hashes (TextLine& text_line)
{
  printf ("computed: text_hash=%08x text_unicode_hash=%08x\n", text_line.TextHash (), text_line.TextUnicodeHash ());
  printf ("real:     text_hash=%08x text_unicode_hash=%08x\n", common::strhash (text_line.Text ()), common::strhash (text_line.TextUnicode ()));
}

int main ()
{
  printf ("Results of text_line_hash_test:\n");

  TextLine::Pointer text_line (TextLine::Create ());

  print_hashes (*text_line);
  
  printf ("Set text\n");

  text_line->SetText ("Hello world");
  
  print_hashes (*text_line);  
  
  printf ("Set unicode text\n");
  
  text_line->SetText (L"Hello world");  
  
  print_hashes (*text_line);
  
  return 0;
}
