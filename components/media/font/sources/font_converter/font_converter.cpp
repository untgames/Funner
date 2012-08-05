#include <cmath>

#include <stl/hash_map>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/string.h>
#include <xtl/uninitialized_storage.h>

#include <common/file.h>
#include <common/hash.h>
#include <common/log.h>

#include <media/atlas_builder.h>
#include <media/font_converter.h>
#include <media/image.h>

#include <ft2build.h>
#include <freetype/freetype.h>

namespace
{

const size_t RESOLUTION = 72;

const media::PixelFormat RESULT_IMAGE_FORMAT = media::PixelFormat_L8;

const char* LOG_NAME = "media.font.font_converter"; //имя потока протоколирования

class FreeTypeLibrary
{
  public:
    FreeTypeLibrary () 
    {
      if (FT_Init_FreeType (&freetype_library))
        throw xtl::format_operation_exception ("FreeTypeLibrary::FreeTypeLibrary", "Can't init freetype library\n");
    }

    ~FreeTypeLibrary ()
    {
      FT_Done_FreeType (freetype_library);
    }

    FT_Library& Library () 
    { 
      return freetype_library; 
    }

  private:
    FT_Library freetype_library;
};

class FontFace
{
  public:
    FontFace (const char* font_file_name, FT_Library& freetype_library)
    {
      common::InputFile font_file (font_file_name);

      font_data.resize (font_file.Size (), false);

      font_file.Read (font_data.data (), font_data.size ());

      if (FT_New_Memory_Face (freetype_library, (const FT_Byte*)font_data.data (), font_data.size (), 0, &face))
        throw xtl::format_operation_exception ("FontFace::FontFace", "Can't create font face\n");
    }

    ~FontFace ()
    {
      FT_Done_Face (face);
    }

    FT_Face& Face ()
    {
      return face;
    }

  private:
    FT_Face                          face;
    xtl::uninitialized_storage<char> font_data;
};

//получение ближайшей сверху степени двойки
size_t get_next_higher_power_of_two (size_t k) 
{
  if (!k)
    return 1;

  k--;

  for (size_t i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

void set_null_glyph_data (media::GlyphInfo* glyph)
{
  glyph->x_pos = 0;
  glyph->y_pos = 0;
  glyph->width = 0;
  glyph->height = 0;
  glyph->bearing_x = 0;
  glyph->bearing_y = 0;
  glyph->advance_y = 0;
}

size_t get_glyph_bitmap_size (FT_Bitmap_& bitmap)
{
  return abs (bitmap.pitch) * bitmap.rows;
}

typedef stl::hash_map<size_t, size_t>             BitmapHashMap;
typedef stl::vector<stl::pair<size_t, size_t> >   DuplicateGlyphs;

common::Log& get_log ()
{
  static common::Log log (LOG_NAME);
  
  return log;
}

}

namespace media
{

/*
   Конвертация шрифта
*/

void convert (const FontDesc& font_desc, Font& result_font, Image& result_image)
{
  static const char* METHOD_NAME = "media::convert (const FontDesc&, Font&, Image&)";

  if (!font_desc.file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "font_desc.file_name");

  if (!font_desc.char_codes)
    throw xtl::make_null_argument_exception (METHOD_NAME, "font_desc.char_codes");
  
  if (!font_desc.char_codes_count)
    throw xtl::format_operation_exception (METHOD_NAME, "Empty char codes string");
  
  if (!font_desc.glyph_size)
    throw xtl::make_null_argument_exception (METHOD_NAME, "font_desc.glyph_size");

  FreeTypeLibrary freetype_library;

  FontFace font_face (font_desc.file_name, freetype_library.Library ());

  FT_Face& face = font_face.Face ();

  if (FT_Select_Charmap (face, FT_ENCODING_UNICODE))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set unicode char map");

  size_t freetype_char_size = font_desc.glyph_size * 64;

  if (FT_Set_Char_Size (face, freetype_char_size, freetype_char_size, RESOLUTION, RESOLUTION))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set char size");

  const AtlasBuilder::PackHandler& pack_handler = AtlasBuilderManager::GetPacker ("default");

    //Установка данных шрифта

  media::Font font;

  font.Rename (result_font.Name ());
  font.SetImageName (result_image.Name ());
  font.ResizeGlyphsTable (font_desc.char_codes_count);
  font.SetFirstGlyphCode (font_desc.first_glyph_code);

    //Подготовка массива с размерами каждого глифа

  size_t glyphs_count = 0;

  xtl::uninitialized_storage<math::vec2ui> glyph_sizes (font_desc.char_codes_count), glyph_origins (font_desc.char_codes_count);
  xtl::uninitialized_storage<size_t>       glyph_indices (font_desc.char_codes_count);
  xtl::uninitialized_storage<FT_UInt>      ft_char_indices (font_desc.char_codes_count);

  for (size_t i = 0; i < font_desc.char_codes_count; i++)
    ft_char_indices.data () [i] = FT_Get_Char_Index (face, font_desc.char_codes [i]);

  media::GlyphInfo *current_glyph = font.Glyphs ();

  memset (current_glyph, 0, sizeof (media::GlyphInfo) * font.GlyphsTableSize ());

  BitmapHashMap   bitmap_map;
  DuplicateGlyphs duplicate_glyphs;

  duplicate_glyphs.reserve (font_desc.char_codes_count);
  
  for (size_t i = 0; i < font_desc.char_codes_count; i++, current_glyph++)
  {
    size_t  char_code  = font_desc.char_codes [i];
    FT_UInt char_index = ft_char_indices.data () [i];

    if (!char_index)
    {
      set_null_glyph_data (current_glyph);

      get_log ().Printf ("Font '%s' has no char %u.", font_desc.file_name, char_code);

      continue;
    }

    if (FT_Load_Char (face, char_code, FT_LOAD_RENDER))
    {
      set_null_glyph_data (current_glyph);

      get_log ().Printf ("Can't load char %u.", char_code);
    
      continue;
    }

    if (!face->glyph->bitmap.buffer)
    {
      get_log ().Printf ("Freetype returned null for character %u.", char_code);
      
      set_null_glyph_data (current_glyph);
      current_glyph->advance_x = face->glyph->metrics.horiAdvance >> 6;

      continue;
    }

    if (face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
      throw xtl::format_not_supported_exception (METHOD_NAME, "Can't save image, pixel format differs from FT_PIXEL_MODE_GRAY.");

    current_glyph->width     = face->glyph->bitmap.width;
    current_glyph->height    = face->glyph->bitmap.rows;
    current_glyph->bearing_x = face->glyph->metrics.horiBearingX >> 6;
    current_glyph->bearing_y = face->glyph->metrics.horiBearingY >> 6;
    current_glyph->advance_x = face->glyph->metrics.horiAdvance >> 6;
    current_glyph->advance_y = 0;

    size_t bitmap_hash = common::crc32 (face->glyph->bitmap.buffer, get_glyph_bitmap_size (face->glyph->bitmap));

    BitmapHashMap::iterator iter = bitmap_map.find (bitmap_hash);

    if (iter != bitmap_map.end ())
    {
      duplicate_glyphs.push_back (stl::pair<size_t, size_t> (i, iter->second));

      continue;
    }

    bitmap_map [bitmap_hash] = i;
    
    glyph_sizes.data () [glyphs_count].x = face->glyph->bitmap.width + font_desc.glyph_interval;
    glyph_sizes.data () [glyphs_count].y = face->glyph->bitmap.rows + font_desc.glyph_interval;
    glyph_indices.data () [glyphs_count] = i;

    glyphs_count++;
  }

    //Формирование позиций глифов

  size_t pack_flags = AtlasPackFlag_PowerOfTwoEdges;

  if (font_desc.fast_convert)
    pack_flags |= AtlasPackFlag_Fast;

  get_log ().Printf ("packing %u glyphs...", glyphs_count);

  media::AtlasBuilder::PackHandlerParams pack_params;

  memset (&pack_params, 0, sizeof (pack_params));

  pack_params.images_count = glyphs_count;
  pack_params.in_sizes     = glyph_sizes.data ();
  pack_params.out_origins  = glyph_origins.data ();
  pack_params.pack_flags   = pack_flags;

  pack_handler (pack_params);

  get_log ().Printf ("building image...");

  size_t result_image_width = 0, result_image_height = 0;

  for (size_t i = 0; i < glyphs_count; i++)
  {
    if (result_image_width < (glyph_origins.data ()[i].x + glyph_sizes.data ()[i].x))
      result_image_width = glyph_origins.data ()[i].x + glyph_sizes.data ()[i].x;
    if (result_image_height < (glyph_origins.data ()[i].y + glyph_sizes.data ()[i].y))
      result_image_height = glyph_origins.data ()[i].y + glyph_sizes.data ()[i].y;
  }

  result_image_width  = get_next_higher_power_of_two (result_image_width);
  result_image_height = get_next_higher_power_of_two (result_image_height);

  media::Image image (result_image_width, result_image_height, 1, RESULT_IMAGE_FORMAT);

  image.Rename (result_image.Name ());

  unsigned char* image_data = (unsigned char*)image.Bitmap ();

    //Сброс цвета (чёрный, прозрачный)
  
  size_t bytes_per_pixel = get_bytes_per_pixel (RESULT_IMAGE_FORMAT);
  size_t row_size = bytes_per_pixel * result_image_width;

  memset (image_data, 0, result_image_width * result_image_height * bytes_per_pixel);

    //Формирование конечного изображения

  for (size_t i = 0; i < glyphs_count; i++)
  {
     FT_ULong char_code = font_desc.char_codes [glyph_indices.data () [i]];

     if (FT_Load_Char (face, char_code, FT_LOAD_RENDER))
      throw xtl::format_operation_exception (METHOD_NAME, "Can't render glyph %u second time", font_desc.char_codes [glyph_indices.data () [i]]);

    if (!face->glyph->bitmap.buffer)
      throw xtl::format_operation_exception (METHOD_NAME, "Can't get glyph %u buffer while rendering second time", font_desc.char_codes [glyph_indices.data () [i]]);

    current_glyph = font.Glyphs () + glyph_indices.data () [i];

    size_t origin_x = glyph_origins.data () [i].x;
    size_t origin_y = glyph_origins.data () [i].y;

    current_glyph->x_pos = origin_x;
    current_glyph->y_pos = origin_y;

    for (int j = 0; j < face->glyph->bitmap.rows; j++)
    {
      unsigned char* buffer = face->glyph->bitmap.buffer + face->glyph->bitmap.pitch * j;
      unsigned char* destination_pixel = &image_data[((origin_y + (face->glyph->bitmap.rows - j - 1)) * row_size) + origin_x * bytes_per_pixel];

      for (int k = 0; k < face->glyph->bitmap.width; k++, destination_pixel += bytes_per_pixel, buffer++)
        memset (destination_pixel, *buffer, bytes_per_pixel);
    }
  }

    //Формирование данных повторяющихся глифов

  for (DuplicateGlyphs::iterator iter = duplicate_glyphs.begin (), end = duplicate_glyphs.end (); iter != end; ++iter)
  {
    GlyphInfo *first_glyph_occurence = font.Glyphs () + iter->second;
    GlyphInfo *same_glyph            = font.Glyphs () + iter->first;

    same_glyph->x_pos = first_glyph_occurence->x_pos;
    same_glyph->y_pos = first_glyph_occurence->y_pos;
  }

  get_log ().Printf ("getting kernings...");

    //Формирование кёрнингов

  for (size_t i = 0; i < font_desc.char_codes_count; i++)
  {
    if (font_desc.char_codes [i] != i + font_desc.first_glyph_code)
      continue;

    for (size_t j = 0; j < font_desc.char_codes_count; j++)
    {
      if (font_desc.char_codes [j] != j + font_desc.first_glyph_code)
        continue;

      FT_Vector kerning;

      if (FT_Get_Kerning (face, ft_char_indices.data () [i], ft_char_indices.data () [j], FT_KERNING_UNFITTED, &kerning))
      {
        get_log ().Printf ("Can't get kerning for pair %u-%u.", i, j);
        continue;
      }

      if (kerning.x || kerning.y)
      {
        media::KerningInfo kerning_info;

        kerning_info.x_kerning = float (kerning.x >> 6);
        kerning_info.y_kerning = float (kerning.y >> 6);

        font.InsertKerning (i, j, kerning_info);
      }
    }
  }

  swap (result_image, image);
  swap (result_font,  font);
}

}
