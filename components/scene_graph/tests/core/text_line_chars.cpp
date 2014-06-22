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

int main ()
{
  try
  {
    printf ("Results of text_line_chars_test:\n");

    media::FontLibrary font_library;

    font_library.LoadFont ("data/times.xfont");

    TextLine::Pointer text_line (TextLine::Create (font_library));

    media::FontCreationParams creation_params = text_line->FontCreationParams ();

    creation_params.font_size = 50;

    text_line->SetTextUtf8 ("Hello world");
    text_line->SetFont  ("Times New Roman");
    text_line->SetFontCreationParams (creation_params);
    text_line->SetSpacingMultiplier (2.f);

    size_t chars_count = text_line->CharsCount ();

    const CharDesc* cur_char = text_line->Chars ();

    printf ("Text line has %u chars:\n", chars_count);

    for (size_t i=0; i<chars_count; i++, cur_char++)
    {
      printf ("  code=%03u font='%s' position=[%.2f %.2f %.2f] size=[%.2f %.2f] color=[%.2f %.2f %.2f %.2f]\n",
        cur_char->code, cur_char->font ? cur_char->font->Name () : "", cur_char->position.x, cur_char->position.y, cur_char->position.z,
        cur_char->size.x, cur_char->size.y, cur_char->color.x, cur_char->color.y, cur_char->color.z, cur_char->color.w);
    }
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
