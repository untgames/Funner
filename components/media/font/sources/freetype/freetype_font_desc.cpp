#include "shared.h"

using namespace media;
using namespace media::freetype;

namespace
{

const char* LOG_NAME = "media.freetype.FreetypeFontDesc";

typedef xtl::uninitialized_storage<char> DataBuffer;
typedef xtl::shared_ptr<DataBuffer>      DataBufferPtr;

//Шрифт free type
class FreetypeFace : public common::Lockable, public xtl::reference_counter
{
  public:
    FreetypeFace (const DataBufferPtr& in_data, const FreetypeLibrary& in_library, size_t face_index)
      : data (in_data)
      , library (in_library)
      , face (0)
      , current_size (0)
      , current_horizontal_dpi (0)
      , current_vertical_dpi (0)
    {
      library.FT_New_Memory_Face ((const FT_Byte*)data->data (), data->size (), face_index, &face);
      library.FT_Select_Charmap (face, FT_ENCODING_UNICODE);
    }

    ~FreetypeFace ()
    {
      try
      {
        if (face)
        {
          library.FT_Done_Face (face);
        }
      }
      catch (xtl::exception& e)
      {
        common::Log (LOG_NAME).Printf ("Can't destroy freetype face, exception '%s'", e.what ());
      }
    }

    FT_Face FaceHandle ()
    {
      return face;
    }

    //Получение ближайшего доступного размера шрифта, если такой не найден - возвращает 0
    size_t GetNearestFontSize (size_t size, size_t size_eps)
    {
      if (face->face_flags & FT_FACE_FLAG_SCALABLE)
        return size;

      if (!face->num_fixed_sizes)
        return 0;

      size_t min_size = size > size_eps ? size - size_eps : 1;
      size_t max_size = size + size_eps;

      if (max_size < size)
        max_size = (size_t)-1;

      size_t best_size;
      size_t size_diff  = (size_t)-1;
      bool   size_found = false;

      for (size_t i = 0; i < face->num_fixed_sizes; i++)
      {
        size_t current_size = face->available_sizes [i].width;

        if (current_size >= min_size && current_size <= max_size)
        {
          size_found = true;

          size_t current_size_diff = size > current_size ? size - current_size : current_size - size;

          if (!current_size_diff)
            return size;

          if (current_size_diff < size_diff)
          {
            size_diff = current_size_diff;
            best_size = current_size;
          }
        }
      }

      return size_found ? best_size : 0;
    }

    //Установка размера шрифта
    void SetSize (size_t size, size_t horizontal_dpi, size_t vertical_dpi)
    {
      if (size == current_size && horizontal_dpi == current_horizontal_dpi && vertical_dpi == current_vertical_dpi)
        return;

      library.FT_Set_Char_Size (face, size << 6, 0, horizontal_dpi, vertical_dpi);

      current_size           = size;
      current_horizontal_dpi = horizontal_dpi;
      current_vertical_dpi   = vertical_dpi;
    }

  private:
    FreetypeFace (const FreetypeFace&);             //no impl
    FreetypeFace& operator = (const FreetypeFace&); //no impl

  private:
    DataBufferPtr   data;                    //данные файла шрифта
    FreetypeLibrary library;                 //библиотека
    FT_Face         face;                    //шрифт
    size_t          current_size;            //текущий установленный размер шрифта
    size_t          current_horizontal_dpi;  //текущее установленное разрешение целевого устройства вывода
    size_t          current_vertical_dpi;    //текущее установленное разрешение целевого устройства вывода
};

typedef xtl::intrusive_ptr<FreetypeFace> FreetypeFacePtr;
typedef stl::vector<FreetypeFacePtr>     FacesArray;

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

    const char* charset = CharsetManager::FindCharset (params.charset_name);

    if (!charset)
      throw xtl::make_argument_exception ("", "params.charset");

      //convert charset to utf32
    size_t                    charset_length = xtl::xstrlen (charset);
    stl::vector<unsigned int> utf32_charset (charset_length);

    const void* source_ptr  = charset;
    size_t      source_size = charset_length;
    void*       dst_ptr     = &utf32_charset.front ();
    size_t      dst_size    = utf32_charset.size () * sizeof (int);

    common::convert_encoding (common::Encoding_UTF8, source_ptr, source_size, common::Encoding_UTF32LE, dst_ptr, dst_size);

    if (source_size)
      throw xtl::format_operation_exception ("", "Can't convert charset to utf32");

    size_t charset_size = utf32_charset.size () - dst_size / sizeof (int);

    utf32_charset.resize (charset_size);

    stl::sort (utf32_charset.begin (), utf32_charset.end ());

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

    if (charset_size)
    {
      unsigned int first_glyph_code = utf32_charset.front ();

      builder.SetFirstGlyphCode (first_glyph_code);

      size_t glyphs_count = utf32_charset.back () - utf32_charset.front () + 1;

      builder.SetGlyphsCount (glyphs_count);

      xtl::uninitialized_storage<FT_UInt> ft_char_indices (charset_size);

      for (size_t i = 0; i < charset_size; i++)
        ft_char_indices.data () [i] = impl->library.FT_Get_Char_Index (face_handle, utf32_charset [i]);

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

        size_t previous_glyph_code = utf32_charset.front ();

        GlyphInfo *current_glyph = builder.Glyphs ();

        for (size_t i = 0; i < charset_size; i++, current_glyph++)
        {
          size_t char_code = utf32_charset [i];

          for (size_t j = previous_glyph_code + 1; j < char_code; j++, current_glyph++)
            memcpy (current_glyph, &null_glyph, sizeof (GlyphInfo));

          previous_glyph_code = char_code;

          FT_UInt char_index = ft_char_indices.data () [i];

          if (!char_index)
          {
            memcpy (current_glyph, &null_glyph, sizeof (GlyphInfo));

            impl->log.Printf ("Font '%s' has no char %lu.", impl->source.c_str (), char_code);

            continue;
          }

          if (!impl->library.FT_Load_Char (face_handle, char_code, FT_LOAD_DEFAULT, true))
          {
            memcpy (current_glyph, &null_glyph, sizeof (GlyphInfo));

            impl->log.Printf ("Can't load char %lu.", char_code);

            continue;
          }

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
              impl->log.Printf ("Can't get kerning for pair %u-%u.", utf32_charset [i], utf32_charset [j]);
              continue;
            }

            if (kerning.x || kerning.y)
            {
              media::KerningInfo kerning_info;

              kerning_info.x_kerning = kerning.x / 64.f;
              kerning_info.y_kerning = kerning.y / 64.f;

              builder.InsertKerning (utf32_charset [i] - first_glyph_code, utf32_charset [j] - first_glyph_code, kerning_info);
            }
          }
        }
      }
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
