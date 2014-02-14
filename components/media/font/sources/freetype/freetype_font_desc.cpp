#include "shared.h"

using namespace media;
using namespace media::freetype;

namespace
{

const char* LOG_NAME = "media.freetype.FreetypeFontDesc";

typedef stl::vector<FreetypeFacePtr> FacesArray;

}

namespace media
{

namespace freetype
{

/*
   Шрифт
*/

struct FreetypeFontDesc::Impl
{
  common::Log     log;       //протокол
  DataBufferPtr   font_data; //данные файла шрифта
  FreetypeLibrary library;   //freetype библиотека, создаем каждый раз новую для возможной работы со шрифтами в разных нитях
  FacesArray      faces;     //шрифты
  stl::string     source;    //исходный файл

  ///Конструктор / деструктор
  Impl (const char* file_name)
    : log (LOG_NAME)
    , source (file_name)
  {
    try
    {
      common::InputFile font_file (file_name);

      font_data = DataBufferPtr (new DataBuffer (font_file.Size ()));

      font_file.Read (font_data->data (), font_data->size ());

      FT_Face test_face;

      library.FT_New_Memory_Face ((const FT_Byte*)font_data->data (), font_data->size (), -1, &test_face);

      FT_Long faces_count = test_face->num_faces;

      library.FT_Done_Face (test_face);

      faces.reserve (faces_count);

      for (size_t i = 0; i < faces_count; i++)
        faces.push_back (FreetypeFacePtr (new FreetypeFace (font_data, library, i), false));
    }
    catch (xtl::exception& e)
    {
      e.touch ("media::freetype::FreetypeFontDesc::FreetypeFontDesc");
      throw;
    }
  }
};

}

}

/*
   Конструктор / деструктор
*/

FreetypeFontDesc::FreetypeFontDesc (const char* file_name)
  : impl (new Impl (file_name))
  {}

FreetypeFontDesc::~FreetypeFontDesc ()
{
  delete impl;
}

/*
   Количество шрифтов в наборе
*/

size_t FreetypeFontDesc::FontsCount ()
{
  return impl->faces.size ();
}

/*
   Имя гарнитуры / имя семейства / имя стиля
*/

const char* FreetypeFontDesc::FamilyName (size_t index)
{
  if (index > impl->faces.size ())
    throw xtl::make_range_exception ("media::freetype::FreetypeFontDesc::FamilyName", "index", index, 0u, impl->faces.size ());

  return impl->faces [index]->FaceHandle ()->family_name;
}

const char* FreetypeFontDesc::StyleName (size_t index)
{
  if (index > impl->faces.size ())
    throw xtl::make_range_exception ("media::freetype::FreetypeFontDesc::StyleName", "index", index, 0u, impl->faces.size ());

  return impl->faces [index]->FaceHandle ()->style_name;
}

/*
   Создание шрифта
*/

Font FreetypeFontDesc::CreateFont (size_t index, const FontCreationParams& params)
{
  try
  {
    if (index > impl->faces.size ())
      throw xtl::make_range_exception ("", "index", index, 0u, impl->faces.size ());

    if (!CanCreateFont (index, params))
      throw xtl::make_argument_exception ("", "params");

    const unsigned int* charset = CharsetManager::FindSortedUtf32Charset (params.charset_name);

    if (!charset)
      throw xtl::make_argument_exception ("", "params.charset");

    size_t charset_size = 0;

    for (const unsigned int* current_char = charset; *current_char; current_char++)
      charset_size++;

    RasterizedFontParamsPtr rasterized_font_params (new RasterizedFontParams, false);

    rasterized_font_params->library = impl->library;

    memcpy (&rasterized_font_params->font_params, &params, sizeof (params));

    CharCodesBuffer& utf32_charset = rasterized_font_params->utf32_charset;

    utf32_charset.resize (charset_size);

    memcpy (utf32_charset.data (), charset, charset_size * sizeof (unsigned int));

      //fill result font
    FreetypeFacePtr face        = impl->faces [index];
    FT_Face         face_handle = face->FaceHandle ();

    FontBuilder builder;

    builder.SetSource     (impl->source.c_str ());
    builder.Rename        (common::format ("%s %s", face_handle->family_name, face_handle->style_name).c_str ());
    builder.SetFamilyName (face_handle->family_name);
    builder.SetStyleName  (face_handle->style_name);

    size_t choosen_size = face->GetNearestFontSize (params.font_size, params.font_size_eps);

    builder.SetFontSize (choosen_size);

    rasterized_font_params->choosen_size = choosen_size;
    rasterized_font_params->face         = face;

    if (charset_size)
    {
      unsigned int first_glyph_code = utf32_charset.data () [0];

      builder.SetFirstGlyphCode (first_glyph_code);

      size_t glyphs_count = utf32_charset.data () [charset_size - 1] - first_glyph_code + 1;

      builder.SetGlyphsCount (glyphs_count);

      xtl::uninitialized_storage<FT_UInt>& ft_char_indices = rasterized_font_params->ft_char_indices;

      ft_char_indices.resize (charset_size);

      for (size_t i = 0; i < charset_size; i++)
        ft_char_indices.data () [i] = impl->library.FT_Get_Char_Index (face_handle, utf32_charset.data () [i]);

        //Get glyphs data
      {
        common::Lock lock (*face);

        face->SetSize (choosen_size, params.horizontal_dpi, params.vertical_dpi);

        GlyphInfo null_glyph;

        if (impl->library.FT_Load_Char (face_handle, '?', FT_LOAD_DEFAULT, true))
        {
          null_glyph.width     = face_handle->glyph->metrics.width / 64.f;
          null_glyph.height    = face_handle->glyph->metrics.height / 64.f;
          null_glyph.bearing_x = face_handle->glyph->metrics.horiBearingX / 64.f;
          null_glyph.bearing_y = face_handle->glyph->metrics.horiBearingY / 64.f;
          null_glyph.advance_x = face_handle->glyph->metrics.horiAdvance / 64.f;
          null_glyph.advance_y = 0;
        }
        else
          memset (&null_glyph, 0, sizeof (null_glyph));

        size_t previous_glyph_code = first_glyph_code;

        GlyphInfo          *current_glyph     = builder.Glyphs ();
        const unsigned int *current_char_code = utf32_charset.data ();

        for (size_t i = 0; i < charset_size; i++, current_glyph++, current_char_code++)
        {
          if (current_glyph - builder.Glyphs () >= *current_char_code)
            exit (0);

          for (size_t j = previous_glyph_code + 1; j < *current_char_code; j++, current_glyph++)
            memcpy (current_glyph, &null_glyph, sizeof (GlyphInfo));

          previous_glyph_code = *current_char_code;

          FT_UInt char_index = ft_char_indices.data () [i];

          if (!char_index)
          {
            memcpy (current_glyph, &null_glyph, sizeof (GlyphInfo));

            impl->log.Printf ("Font '%s' has no char %lu.", impl->source.c_str (), *current_char_code);

            continue;
          }

          if (!impl->library.FT_Load_Char (face_handle, *current_char_code, FT_LOAD_DEFAULT, true))
          {
            memcpy (current_glyph, &null_glyph, sizeof (GlyphInfo));

            impl->log.Printf ("Can't load char %lu.", *current_char_code);

            continue;
          }

          //TODO проверить, есть ли прирост скорости, если сохрянать глифы и для рендеринга использовать FT_Glyph_To_Bitmap

          current_glyph->width     = face_handle->glyph->metrics.width / 64.f;
          current_glyph->height    = face_handle->glyph->metrics.height / 64.f;
          current_glyph->bearing_x = face_handle->glyph->metrics.horiBearingX / 64.f;
          current_glyph->bearing_y = face_handle->glyph->metrics.horiBearingY / 64.f;
          current_glyph->advance_x = face_handle->glyph->metrics.horiAdvance / 64.f;
          current_glyph->advance_y = 0;
        }

          //Формирование кёрнингов

        for (size_t i = 0; i < charset_size; i++)
        {
          for (size_t j = 0; j < charset_size; j++)
          {
            FT_Vector kerning;

            if (!impl->library.FT_Get_Kerning (face_handle, ft_char_indices.data () [i], ft_char_indices.data () [j], FT_KERNING_UNFITTED, &kerning, true))
            {
              impl->log.Printf ("Can't get kerning for pair %u-%u.", utf32_charset.data () [i], utf32_charset.data () [j]);
              continue;
            }

            if (kerning.x || kerning.y)
            {
              media::KerningInfo kerning_info;

              kerning_info.x_kerning = kerning.x / 64.f;
              kerning_info.y_kerning = kerning.y / 64.f;

              builder.InsertKerning (utf32_charset.data () [i] - first_glyph_code, utf32_charset.data () [j] - first_glyph_code, kerning_info);
            }
          }
        }
      }

      builder.SetRasterizer (FreetypeRasterizedFont (rasterized_font_params));
    }

    return builder.Font ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::freetype::FreetypeFontDesc::CreateFont");
    throw;
  }
}

bool FreetypeFontDesc::CanCreateFont (size_t index, const FontCreationParams& params)
{
  if (index > impl->faces.size ())
    throw xtl::make_range_exception ("media::freetype::FreetypeFontDesc::CanCreateFont", "index", index, 0u, impl->faces.size ());

  if (!impl->faces [index]->GetNearestFontSize (params.font_size, params.font_size_eps))
    return false;

  if (params.weight != FontWeight_Normal)  //not implemented
    return false;

  if (params.escapement)  //not implemented
    return false;

  if (params.bold)  //not implemented
    return false;

  if (params.italic)  //not implemented
    return false;

  if (params.underlined)  //not implemented
    return false;

  if (params.striked)  //not implemented
    return false;

  if (params.stroke_size)  //not implemented
    return false;

  return true;
}
