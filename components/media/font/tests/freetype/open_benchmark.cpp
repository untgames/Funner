#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/uninitialized_storage.h>

#include <common/file.h>
#include <common/time.h>

#include <ft2build.h>
#include <freetype/freetype.h>

const char*  FILE_NAME               = "data/times_new_roman.ttf";
const size_t MEMORY_ITERATIONS_COUNT = 40000;
const size_t FILE_ITERATIONS_COUNT   = 10000;

int main ()
{
  printf ("Results of open_benchmark:\n");

  try
  {
    FT_Library library;

    if (FT_Init_FreeType (&library))
      throw xtl::format_operation_exception ("::FT_Init_FreeType", "Can't init freetype library\n");

    {
      common::InputFile font_file (FILE_NAME);

      xtl::uninitialized_storage<char> font_data (font_file.Size ());

      font_file.Read (font_data.data (), font_data.size ());

      size_t start_time = common::milliseconds ();

      for (size_t i = 0; i < MEMORY_ITERATIONS_COUNT; i++)
      {
        FT_Face face;

        if (FT_New_Memory_Face (library, (const FT_Byte*)font_data.data (), font_data.size (), 0, &face))
          throw xtl::format_operation_exception ("FontFace::FontFace", "Can't create font face\n");

        FT_Done_Face (face);
      }

      printf ("FreeType font creation speed from memory is %f/s\n", MEMORY_ITERATIONS_COUNT / ((common::milliseconds () - start_time) / 1000.f));
    }

    {
      size_t start_time = common::milliseconds ();

      for (size_t i = 0; i < FILE_ITERATIONS_COUNT; i++)
      {
        common::InputFile font_file (FILE_NAME);

        xtl::uninitialized_storage<char> font_data (font_file.Size ());

        font_file.Read (font_data.data (), font_data.size ());

        FT_Face face;

        if (FT_New_Memory_Face (library, (const FT_Byte*)font_data.data (), font_data.size (), 0, &face))
          throw xtl::format_operation_exception ("FontFace::FontFace", "Can't create font face\n");

        FT_Done_Face (face);
      }

      printf ("FreeType font creation speed from file is %f/s\n", FILE_ITERATIONS_COUNT / ((common::milliseconds () - start_time) / 1000.f));
    }

    FT_Done_FreeType (library);
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
