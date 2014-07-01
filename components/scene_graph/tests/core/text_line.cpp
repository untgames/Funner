#include "shared.h"

stl::wstring towstring (const TextLine& line)
{
  if (sizeof (wchar_t) == 4)
    return stl::wstring ((const wchar_t*)line.TextUtf32 (), line.TextLength ());

  stl::wstring result;
  
  result.fast_resize (line.TextLength ());

  const void* source           = line.TextUtf32 ();
  size_t      source_size      = line.TextLength () * sizeof (unsigned int);
  void*       destination      = &result [0];
  size_t      destination_size = result.size () * sizeof (wchar_t);

  convert_encoding (common::Encoding_UTF32LE, source, source_size, common::Encoding_UTF16LE, destination, destination_size);  
  
  return result;
}

const char* get_name (TextLineAlignment alignment)
{
  switch (alignment)
  {
    case TextLineAlignment_Center:   return "TextLineAlignment_Center";
    case TextLineAlignment_Left:     return "TextLineAlignment_Left/TextLineAlignment_Top";
    case TextLineAlignment_Right:    return "TextLineAlignment_Right/TextLineAlignment_Bottom";
    case TextLineAlignment_BaseLine: return "TextLineAlignment_BaseLine";
    default: return "Invalid";
  }
}

int main ()
{
  printf ("Results of text_line_test:\n");

  media::FontLibrary font_library;

  TextLine::Pointer text_line (TextLine::Create (font_library));

  printf ("TextLine color = [%.2f %.2f %.2f %.2f]\n", text_line->Color ().x, text_line->Color ().y, text_line->Color ().z, text_line->Color ().w);
  printf ("TextLine text = '%s'\n", text_line->TextUtf8 ());
  printf ("TextLine unicode text = '%S'\n", towstring (*text_line).c_str ());
  printf ("TextLine font name = '%s'\n", text_line->Font ());
  printf ("TextLine length = %u\n", text_line->TextLength ());
  printf ("TextLine horizontal alignment = '%s', vertical_alignment = '%s'\n", get_name (text_line->HorizontalAlignment ()), get_name (text_line->VerticalAlignment ()));
  printf ("textLine spacing multiplier = %.2f\n", text_line->SpacingMultiplier ());

  text_line->SetColor (0.1f, 0.2f, 0.3f, 0.4f);
  text_line->SetTextUtf8 ("Non-unicode text");
  text_line->SetFont  ("font");
  text_line->SetAlignment (TextLineAlignment_Center, TextLineAlignment_Bottom);
  text_line->SetSpacingMultiplier (2.f);

  printf ("TextLine color = [%.2f %.2f %.2f %.2f]\n", text_line->Color ().x, text_line->Color ().y, text_line->Color ().z, text_line->Color ().w);
  printf ("TextLine text = '%s'\n", text_line->TextUtf8 ());
  printf ("TextLine unicode text = '%S'\n", towstring (*text_line).c_str ());
  printf ("TextLine font name = '%s'\n", text_line->Font ());
  printf ("TextLine length = %u\n", text_line->TextLength ());
  printf ("TextLine horizontal alignment = '%s', vertical_alignment = '%s'\n", get_name (text_line->HorizontalAlignment ()), get_name (text_line->VerticalAlignment ()));
  printf ("textLine spacing multiplier = %.2f\n", text_line->SpacingMultiplier ());
  
  stl::basic_string<unsigned int> result = toutf32 (L"Unicode text");

  text_line->SetTextUtf32 (result.c_str (), result.size ());

  printf ("TextLine text = '%s'\n", text_line->TextUtf8 ());
  printf ("TextLine unicode text = '%S'\n", towstring (*text_line).c_str ());
  printf ("TextLine length = %u\n", text_line->TextLength ());

  text_line->SetHorizontalAlignment (TextLineAlignment_Right);
  text_line->SetVerticalAlignment   (TextLineAlignment_Center);

  printf ("TextLine horizontal alignment = '%s', vertical_alignment = '%s'\n", get_name (text_line->HorizontalAlignment ()), get_name (text_line->VerticalAlignment ()));

  text_line->SetHorizontalAlignment (TextLineAlignment_BaseLine);
  text_line->SetVerticalAlignment   (TextLineAlignment_BaseLine);

  printf ("TextLine horizontal alignment = '%s', vertical_alignment = '%s'\n", get_name (text_line->HorizontalAlignment ()), get_name (text_line->VerticalAlignment ()));

  return 0;
}
