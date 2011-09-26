#include "shared.h"

void print_hashes (TextLine& text_line)
{
  printf ("computed: text_hash=%08x text_unicode_hash=%08x\n", text_line.TextUtf8Hash (), text_line.TextUtf32Hash ());
  printf ("real:     text_hash=%08x text_unicode_hash=%08x\n", common::strhash (text_line.TextUtf8 ()), common::crc32 (text_line.TextUtf32 (), text_line.TextLength () * sizeof (unsigned int)));
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
  
  stl::basic_string<unsigned int> result = toutf32 (L"Hello world");

  text_line->SetText (result.c_str (), result.size ());    
  
  print_hashes (*text_line);
  
  return 0;
}
