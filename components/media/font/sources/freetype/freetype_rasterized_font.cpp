#include "shared.h"

using namespace media;
using namespace media::freetype;

namespace
{

const char* LOG_NAME = "media.freetype.FreetypeRasterizedFont";

}

namespace media
{

namespace freetype
{

/*
   Растеризатор шрифта
*/

struct FreetypeRasterizedFont::Impl : public xtl::reference_counter
{
  typedef xtl::uninitialized_storage<math::vec2ui>   SizesArray;
  typedef xtl::uninitialized_storage<unsigned char*> BitmapPointersArray;
  typedef xtl::uninitialized_storage<unsigned int>   GlyphMap;
  typedef xtl::uninitialized_storage<unsigned char>  BitmapBuffer;
  typedef stl::hash_map<size_t, size_t>              BitmapHashMap;

  common::Log             log;                //протокол
  RasterizedFontParamsPtr font_params;        //параметры шрифта
  size_t                  glyphs_count;       //количество глифов
  bool                    rasterized;         //выполнялась ли процедура растеризации
  SizesArray              sizes;              //размеры битмапов символов
  BitmapPointersArray     bitmap_pointers;    //указатели на битмапы символов
  BitmapHashMap           bitmaps_hash_map;   //карта хешей
  GlyphMap                glyph_map;          //карта соответствия глифов уникальному глифу
  BitmapBuffer            bitmap_data;        //общий буфер для всех битмапов

  ///Конструктор
  Impl (RasterizedFontParamsPtr in_font_params)
    : log (LOG_NAME)
    , font_params (in_font_params)
    , rasterized (false)
  {
    glyphs_count = font_params->utf32_charset.empty () ? 0 : font_params->utf32_charset.data () [font_params->utf32_charset.size () - 1] - font_params->utf32_charset.data () [0] + 1;
  }

  ///Растеризация
  void Rasterize ()
  {
    if (rasterized)
      return;

    const CharCodesBuffer& utf32_charset = font_params->utf32_charset;

    if (utf32_charset.empty ())
      return;

    glyph_map.resize (glyphs_count);

    size_t unique_glyphs_count    = utf32_charset.size () + 1;
    size_t estimated_bitmaps_size = unique_glyphs_count * font_params->choosen_size * font_params->choosen_size / 2;

    sizes.resize       (unique_glyphs_count);
    bitmap_data.resize (estimated_bitmaps_size);

    xtl::uninitialized_storage<size_t> bitmaps_offsets (unique_glyphs_count);

    common::Lock lock (*font_params->face);

    font_params->face->SetSize (font_params->choosen_size, font_params->font_params.horizontal_dpi, font_params->font_params.vertical_dpi);

    FT_Face face_handle = font_params->face->FaceHandle ();

    size_t current_glyph_bitmap_offset = 0;

    math::vec2ui* current_size          = sizes.data ();
    size_t*       current_bitmap_offset = bitmaps_offsets.data ();

    if (font_params->library.FT_Load_Char (face_handle, '?', FT_LOAD_RENDER, true) && face_handle->glyph->bitmap.buffer)
    {
      FT_Bitmap *bitmap = &face_handle->glyph->bitmap;

      current_size->x = bitmap->width;
      current_size->y = bitmap->rows;

      current_glyph_bitmap_offset += CopyBitmapToBuffer (bitmap, 0);

      size_t bitmap_hash = common::crc32 (bitmap_data.data (), bitmap->width * bitmap->rows);

      bitmaps_hash_map [bitmap_hash] = 0;
    }
    else
    {
      current_size->x = 0;
      current_size->y = 0;
    }

    *current_bitmap_offset = 0; //'?' glyph is first in bitmap

    current_size++;
    current_bitmap_offset++;

    size_t              previous_glyph_code  = utf32_charset.data () [0];
    size_t              current_unique_index = 1;
    unsigned int*       current_mapping      = glyph_map.data ();
    const unsigned int* current_char_code    = utf32_charset.data ();

    xtl::uninitialized_storage<FT_UInt>& ft_char_indices = font_params->ft_char_indices;

    for (size_t i = 0, count = utf32_charset.size (); i < count; i++, current_mapping++, current_char_code++)
    {
      for (size_t j = previous_glyph_code + 1; j < *current_char_code; j++, current_mapping++)
        *current_mapping = 0;

      previous_glyph_code = *current_char_code;

      FT_UInt char_index = ft_char_indices.data () [i];

      if (!char_index)
      {
        *current_mapping = 0;

        continue;
      }

      if (!font_params->library.FT_Load_Char (face_handle, *current_char_code, FT_LOAD_RENDER, true) || !face_handle->glyph->bitmap.buffer)
      {
        *current_mapping = 0;

        log.Printf ("Can't render char %lu.", *current_char_code);

        continue;
      }

      FT_Bitmap *bitmap = &face_handle->glyph->bitmap;

      size_t new_offset = CopyBitmapToBuffer (bitmap, current_glyph_bitmap_offset);

      size_t bitmap_hash = common::crc32 (bitmap_data.data () + current_glyph_bitmap_offset, bitmap->width * bitmap->rows);

      BitmapHashMap::iterator iter = bitmaps_hash_map.find (bitmap_hash);

      if (iter != bitmaps_hash_map.end ())
      {
        *current_mapping = iter->second;

        continue;
      }

      *current_mapping = current_unique_index;

      bitmaps_hash_map [bitmap_hash] = current_unique_index;

      current_size->x        = bitmap->width;
      current_size->y        = bitmap->rows;
      *current_bitmap_offset = current_glyph_bitmap_offset;

      current_glyph_bitmap_offset += new_offset;

      current_size++;
      current_bitmap_offset++;
      current_unique_index++;
    }

    size_t actual_unique_glyphs = current_unique_index - 1;

    sizes.resize (actual_unique_glyphs);
    bitmap_data.resize (current_glyph_bitmap_offset);

    bitmap_pointers.resize (actual_unique_glyphs);

    current_bitmap_offset = bitmaps_offsets.data ();

    unsigned char*  bitmap_data_base          = bitmap_data.data ();
    unsigned char** current_dst_bitmap_offset = bitmap_pointers.data ();

    for (size_t i = 0; i < actual_unique_glyphs; i++, current_bitmap_offset++, current_dst_bitmap_offset++)
      *current_dst_bitmap_offset = bitmap_data_base + *current_bitmap_offset;

    rasterized = true;

    printf ("Actual unique glyphs = %lu, estimated unique glyphs = %lu, actual bitmap data size = %lu, estimate bitmap data size = %lu\n", actual_unique_glyphs, unique_glyphs_count, current_glyph_bitmap_offset, estimated_bitmaps_size);
  }

  size_t CopyBitmapToBuffer (FT_Bitmap* bitmap, size_t offset)
  {
    size_t bitmap_size = bitmap->width * bitmap->rows;

    if (offset + bitmap_size > bitmap_data.size ())
      bitmap_data.resize (bitmap_data.size () * 2);

    unsigned char* src_row  = bitmap->pitch > 0 ? bitmap->buffer : bitmap->buffer - bitmap->pitch * bitmap->rows - 1;
    unsigned char* dst_row  = bitmap_data.data () + offset;
    size_t         row_size = bitmap->width;

    if (bitmap->pitch > 0 && bitmap->pitch == bitmap->width)
    {
      memcpy (dst_row, src_row, bitmap->width * bitmap->rows);
    }
    else
    {
      for (int i = 0; i < bitmap->rows; i++, src_row += bitmap->pitch, dst_row += row_size)
        memcpy (dst_row, src_row, row_size);
    }

    return row_size * bitmap->rows;
  }
};

}

}

/*
   Конструктор / деструктор / копирование
*/

FreetypeRasterizedFont::FreetypeRasterizedFont (RasterizedFontParamsPtr font_params)
  : impl (new Impl (font_params))
  {}

FreetypeRasterizedFont::FreetypeRasterizedFont (const FreetypeRasterizedFont& source)
  : impl (source.impl)
{
  addref (impl);
}

FreetypeRasterizedFont::~FreetypeRasterizedFont ()
{
  release (impl);
}

FreetypeRasterizedFont& FreetypeRasterizedFont::operator = (const FreetypeRasterizedFont& source)
{
  FreetypeRasterizedFont (source).Swap (*this);

  return *this;
}

/*
   Получение количества символов шрифта
*/

size_t FreetypeRasterizedFont::GlyphsCount () const
{
  return impl->glyphs_count;
}

/*
   Получение растеризованных глифов (битовые карты глифов - 8 битные монохромные)
*/

size_t FreetypeRasterizedFont::UniqueGlyphsCount () const
{
  impl->Rasterize ();

  return impl->sizes.size ();
}

const unsigned int* FreetypeRasterizedFont::GlyphsMap () const
{
  impl->Rasterize ();

  return impl->glyph_map.data ();
}

const math::vec2ui* FreetypeRasterizedFont::GlyphsSizes () const
{
  impl->Rasterize ();

  return impl->sizes.data ();
}

const unsigned char** FreetypeRasterizedFont::GlyphsBitmaps () const
{
  impl->Rasterize ();

  return (const unsigned char**)impl->bitmap_pointers.data ();
}

/*
   Создание растеризатора
*/

IFontRasterizer* FreetypeRasterizedFont::operator () (const RasterizedFontCreationParams& params)
{
  return new FreetypeFontRasterizer (params, *this);
}

/*
   Обмен
*/

void FreetypeRasterizedFont::Swap (FreetypeRasterizedFont& source)
{
  stl::swap (impl, source.impl);
}
