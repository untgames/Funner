#include "shared.h"

using namespace media;

const char* BAD_FONT_NAME = "data/bad_font.ttf";
const char* FONT_NAME     = "data/times_new_roman.ttf";
//const char* FONT_NAME     = "data/vgafix.fon";

const char* CHARSET_NAME = "charset";

void log_handler (const char* source, const char* message)
{
  printf ("'%s': '%s'\n", source, message);
}

int main ()
{
  printf ("Results of ttf test:\n");

  try
  {
    common::LogFilter filter ("media.*", log_handler);

//    CharsetManager::RegisterCharset (CHARSET_NAME, "ҐґЄєІіЇїАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя „“”«»¡!#$£€¥%'()*+,-–./0123456789:;=?¿@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~©ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþÿıŒœŠšŸŽžğƒˆˇ˘˙˚˛αδεπστφΑAΒBΓGΔDΕEΖZΗHΘQΙIΚKΛLΜMΝNΞCΟOΠPΡRΣSΤTΥUΦFΧXΨYΩWαaβbγgδdεeζzηhθqιiκkλlμmνnξcοoπpρrσsςjVτtυuφfχxψyωwŞşİª’ Ğğ…º&");
    CharsetManager::RegisterCharset (CHARSET_NAME, "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    FontLibrary library;

    library.LoadFont (FONT_NAME);

    printf ("Loaded fonts:\n");

    for (FontLibrary::ConstIterator iter = library.CreateIterator (); iter; ++iter)
    {
      printf ("  %s - %s\n", iter->FamilyName (), iter->StyleName ());

      FontCreationParams creation_params;

      memset (&creation_params, 0, sizeof (creation_params));

      creation_params.font_size      = 12;
      creation_params.font_size_eps  = 10;
      creation_params.weight         = FontWeight_Normal;
      creation_params.escapement     = 0;
      creation_params.bold           = false;
      creation_params.italic         = false;
      creation_params.underlined     = false;
      creation_params.striked        = false;
      creation_params.stroke_size    = 0;
      creation_params.horizontal_dpi = 72;
      creation_params.vertical_dpi   = 72;
      creation_params.charset_name   = CHARSET_NAME;

      Font font = library.CreateFont (iter->FamilyName (), creation_params);

      dump (font);

      RasterizedFontCreationParams rasterization_params;

      rasterization_params.max_image_size = 128;
      rasterization_params.pot            = true;
      rasterization_params.glyph_margin   = 10;
      rasterization_params.image_format   = media::PixelFormat_L8;

      RasterizedFont rasterized_font = font.CreateRasterizedFont (rasterization_params);

      dump (rasterized_font, font);
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
