#include "shared.h"

using namespace media;

const char* FONT_NAME     = "data/times.xfont";
const char* BAD_FONT_NAME = "data/bad_font.xfont";

void log_handler (const char* source, const char* message)
{
  printf ("'%s': '%s'\n", source, message);
}

int main ()
{
  printf ("Results of xfont test:\n");

  try
  {
    common::LogFilter filter ("media.*", log_handler);

    FontLibrary library;

    library.LoadFont (FONT_NAME);

    printf ("Loaded fonts:\n");

    for (FontLibrary::ConstIterator iter = library.CreateIterator (); iter; ++iter)
    {
      printf ("  %s - %s\n", iter->FamilyName (), iter->StyleName ());

      FontCreationParams creation_params;

      memset (&creation_params, 0, sizeof (creation_params));

      creation_params.font_size      = 50;
      creation_params.font_size_eps  = 10;
      creation_params.weight         = FontWeight_Normal;
      creation_params.escapement     = 0;
      creation_params.bold           = false;
      creation_params.italic         = false;
      creation_params.underlined     = false;
      creation_params.striked        = false;
      creation_params.stroke_size    = 1000;
      creation_params.horizontal_dpi = 72;
      creation_params.vertical_dpi   = 72;
      creation_params.charset_name   = "";

      Font font = library.CreateFont (iter->FamilyName (), creation_params);

      dump (font);

      RasterizedFontCreationParams rasterization_params;

      rasterization_params.max_image_size = 1024;
      rasterization_params.pot            = true;
      rasterization_params.glyph_margin   = 10;
      rasterization_params.image_format   = media::PixelFormat_L8;

      RasterizedFont rasterized_font = font.CreateRasterizedFont (rasterization_params);

      dump (rasterized_font, font);

      common::FileSystem::Mkdir ("results");

      for (size_t i = 0; i < rasterized_font.ImagesCount (); i++)
      {
        Image image;

        rasterized_font.BuildImage (i, image);

        image.Save (common::format ("results/result_%d.png", i).c_str ());

        common::FileHash file_hash;

        common::FileSystem::GetFileHash (common::format ("results/result_%d.png", i).c_str (), file_hash);

        printf ("Image %lu hash = %x\n", i, file_hash.crc32);
      }

      common::FileSystem::Remove ("results");
    }

    library.LoadFont (BAD_FONT_NAME);
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
