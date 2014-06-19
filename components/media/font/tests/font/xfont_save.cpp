#include "shared.h"

using namespace media;

const char* FONT_NAME = "data/times.xfont";

void log_handler (const char* source, const char* message)
{
  printf ("'%s': '%s'\n", source, message);
}

int main ()
{
  printf ("Results of xfont save test:\n");

  try
  {
    common::LogFilter filter ("media.*", log_handler);

    FontLibrary library;

    library.LoadFont (FONT_NAME);

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

      RasterizedFontCreationParams rasterization_params;

      rasterization_params.max_image_size = 1024;
      rasterization_params.pot            = true;
      rasterization_params.glyph_margin   = 10;
      rasterization_params.image_format   = media::PixelFormat_L8;

      common::FileSystem::Mkdir ("results");

      common::ComponentLoader loader ("media.font.savers.*");

      FontManager::GetSaver ("xfont") ("results/font.xfont", font, "data/times_%lu.png", rasterization_params);

      common::FileHash file_hash;

      common::FileSystem::GetFileHash (FONT_NAME, file_hash);

      printf ("Original font hash = %x\n", file_hash.crc32);

      common::FileSystem::GetFileHash ("results/font.xfont", file_hash);

      printf ("Saved font hash = %x\n", file_hash.crc32);

      common::FileSystem::Remove ("results");
    }
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
