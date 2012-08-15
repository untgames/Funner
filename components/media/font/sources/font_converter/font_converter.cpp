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
#include <freetype/ftstroke.h>

namespace
{

const size_t RESOLUTION = 72;

const media::PixelFormat RESULT_IMAGE_FORMAT = media::PixelFormat_L8;

const char* LOG_NAME = "media.font.font_converter"; //имя потока протоколирования

const char* get_free_type_error_name (int error)
{
  switch (error)
  {
    case 0x01: return "cannot open resource";
    case 0x02: return "unknown file format";
    case 0x03: return "broken file";
    case 0x04: return "invalid FreeType version";
    case 0x05: return "module version is too low";
    case 0x06: return "invalid argument";
    case 0x07: return "unimplemented feature";
    case 0x08: return "broken table";
    case 0x09: return "broken offset within table";
    case 0x0A: return "array allocation size too large";
    case 0x10: return "invalid glyph index";
    case 0x11: return "invalid character code";
    case 0x12: return "unsupported glyph image format";
    case 0x13: return "cannot render this glyph format";
    case 0x14: return "invalid outline";
    case 0x15: return "invalid composite glyph";
    case 0x16: return "too many hints";
    case 0x17: return "invalid pixel size";
    case 0x20: return "invalid object handle";
    case 0x21: return "invalid library handle";
    case 0x22: return "invalid module handle";
    case 0x23: return "invalid face handle";
    case 0x24: return "invalid size handle";
    case 0x25: return "invalid glyph slot handle";
    case 0x26: return "invalid charmap handle";
    case 0x27: return "invalid cache manager handle";
    case 0x28: return "invalid stream handle";
    case 0x30: return "too many modules";
    case 0x31: return "too many extensions";
    case 0x40: return "out of memory";
    case 0x41: return "unlisted object";
    case 0x51: return "cannot open stream";
    case 0x52: return "invalid stream seek";
    case 0x53: return "invalid stream skip";
    case 0x54: return "invalid stream read";
    case 0x55: return "invalid stream operation";
    case 0x56: return "invalid frame operation";
    case 0x57: return "nested frame access";
    case 0x58: return "invalid frame read";
    case 0x60: return "raster uninitialized";
    case 0x61: return "raster corrupted";
    case 0x62: return "raster overflow";
    case 0x63: return "negative height while rastering";
    case 0x70: return "too many registered caches";
    case 0x80: return "invalid opcode";
    case 0x81: return "too few arguments";
    case 0x82: return "stack overflow";
    case 0x83: return "code overflow";
    case 0x84: return "bad argument";
    case 0x85: return "division by zero";
    case 0x86: return "invalid reference";
    case 0x87: return "found debug opcode";
    case 0x88: return "found ENDF opcode in execution stream";
    case 0x89: return "nested DEFS";
    case 0x8A: return "invalid code range";
    case 0x8B: return "execution context too long";
    case 0x8C: return "too many function definitions";
    case 0x8D: return "too many instruction definitions";
    case 0x8E: return "SFNT font table missing";
    case 0x8F: return "horizontal header (hhea) table missing";
    case 0x90: return "locations (loca) table missing";
    case 0x91: return "name table missing";
    case 0x92: return "character map (cmap) table missing";
    case 0x93: return "horizontal metrics (hmtx) table missing";
    case 0x94: return "PostScript (post) table missing";
    case 0x95: return "invalid horizontal metrics";
    case 0x96: return "invalid character map (cmap) format";
    case 0x97: return "invalid ppem value";
    case 0x98: return "invalid vertical metrics";
    case 0x99: return "could not find context";
    case 0x9A: return "invalid PostScript (post) table format";
    case 0x9B: return "invalid PostScript (post) table";
    case 0xA0: return "opcode syntax error";
    case 0xA1: return "argument stack underflow";
    case 0xA2: return "ignore";
    case 0xB0: return "`STARTFONT' field missing";
    case 0xB1: return "`FONT' field missing";
    case 0xB2: return "`SIZE' field missing";
    case 0xB3: return "`CHARS' field missing";
    case 0xB4: return "`STARTCHAR' field missing";
    case 0xB5: return "`ENCODING' field missing";
    case 0xB6: return "`BBX' field missing";
    case 0xB7: return "`BBX' too big";
    case 0xB8: return "Font header corrupted or missing fields";
    case 0xB9: return "Font glyphs corrupted or missing fields";
    default:   return "unknown error";
  }
}

void check_free_type_error (int error, const char* source)
{
  if (!error)
    return;

  throw xtl::format_operation_exception (source, "Free type error '%s' error code %d", get_free_type_error_name (error), error);
}

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
  try
  {
    if (!font_desc.file_name)
      throw xtl::make_null_argument_exception ("", "font_desc.file_name");

    if (!font_desc.char_codes)
      throw xtl::make_null_argument_exception ("", "font_desc.char_codes");

    if (!font_desc.char_codes_count)
      throw xtl::format_operation_exception ("", "Empty char codes string");

    if (!font_desc.glyph_size)
      throw xtl::make_null_argument_exception ("", "font_desc.glyph_size");

    FreeTypeLibrary freetype_library;

    FontFace font_face (font_desc.file_name, freetype_library.Library ());

    FT_Face& face = font_face.Face ();

    check_free_type_error (FT_Select_Charmap (face, FT_ENCODING_UNICODE), "::FT_Select_Charmap");

    size_t freetype_char_size = font_desc.glyph_size * 64;

    check_free_type_error (FT_Set_Char_Size (face, freetype_char_size, freetype_char_size, RESOLUTION, RESOLUTION), "::FT_Set_Char_Size");

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

    FT_Stroker stroker = 0;
  
    if (font_desc.stroke_width)
    {
      check_free_type_error (FT_Stroker_New (freetype_library.Library (), &stroker), "::FT_Stroker_New");

      FT_Stroker_Set (stroker, (FT_Fixed)(font_desc.stroke_width * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
    }

    int load_char_mode = font_desc.stroke_width ? FT_LOAD_DEFAULT : FT_LOAD_RENDER;

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

      if (FT_Load_Char (face, char_code, load_char_mode))
      {
        set_null_glyph_data (current_glyph);

        get_log ().Printf ("Can't load char %u.", char_code);
      
        continue;
      }

      FT_Glyph  glyph = 0;
      FT_Bitmap *bitmap = 0;

      if (font_desc.stroke_width)
      {
        check_free_type_error (FT_Get_Glyph (face->glyph, &glyph), "::FT_Get_Glyph");

        check_free_type_error (FT_Glyph_Stroke (&glyph, stroker, true), "::FT_Glyph_Stroke");

        if (glyph->format != FT_GLYPH_FORMAT_BITMAP)
          check_free_type_error (FT_Glyph_To_Bitmap (&glyph, FT_RENDER_MODE_NORMAL, 0, 1), "FT_Glyph_To_Bitmap");

        bitmap = &((FT_BitmapGlyph)glyph)->bitmap;
      }
      else
        bitmap = &face->glyph->bitmap;

      if (!bitmap->buffer)
      {
        FT_Done_Glyph (glyph);

        get_log ().Printf ("Freetype returned null for character %u.", char_code);

        set_null_glyph_data (current_glyph);
        current_glyph->advance_x = face->glyph->metrics.horiAdvance >> 6;

        continue;
      }

      if (bitmap->pixel_mode != FT_PIXEL_MODE_GRAY)
      {
        FT_Done_Glyph (glyph);

        throw xtl::format_not_supported_exception ("", "Can't save image, pixel format differs from FT_PIXEL_MODE_GRAY.");
      }

      current_glyph->width     = bitmap->width;
      current_glyph->height    = bitmap->rows;
      current_glyph->bearing_x = face->glyph->metrics.horiBearingX >> 6;
      current_glyph->bearing_y = face->glyph->metrics.horiBearingY >> 6;
      current_glyph->advance_x = face->glyph->metrics.horiAdvance >> 6;
      current_glyph->advance_y = 0;

      size_t bitmap_hash = common::crc32 (bitmap->buffer, get_glyph_bitmap_size (*bitmap));

      BitmapHashMap::iterator iter = bitmap_map.find (bitmap_hash);

      if (iter != bitmap_map.end ())
      {
        FT_Done_Glyph (glyph);

        duplicate_glyphs.push_back (stl::pair<size_t, size_t> (i, iter->second));

        continue;
      }

      bitmap_map [bitmap_hash] = i;

      glyph_sizes.data () [glyphs_count].x = bitmap->width + font_desc.glyph_interval;
      glyph_sizes.data () [glyphs_count].y = bitmap->rows + font_desc.glyph_interval;
      glyph_indices.data () [glyphs_count] = i;

      glyphs_count++;

      FT_Done_Glyph (glyph);
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

       check_free_type_error (FT_Load_Char (face, char_code, load_char_mode), "::FT_Load_Char");

      if (!font_desc.stroke_width && !face->glyph->bitmap.buffer)
        throw xtl::format_operation_exception ("", "Can't get glyph %u buffer while rendering second time", font_desc.char_codes [glyph_indices.data () [i]]);

      current_glyph = font.Glyphs () + glyph_indices.data () [i];

      size_t origin_x = glyph_origins.data () [i].x;
      size_t origin_y = glyph_origins.data () [i].y;

      current_glyph->x_pos = origin_x;
      current_glyph->y_pos = origin_y;

      FT_Glyph  glyph = 0;
      FT_Bitmap *bitmap = 0;

      if (font_desc.stroke_width)
      {
        check_free_type_error (FT_Get_Glyph (face->glyph, &glyph), "::FT_Get_Glyph");

        check_free_type_error (FT_Glyph_Stroke (&glyph, stroker, true), "::FT_Glyph_Stroke");

        if (glyph->format != FT_GLYPH_FORMAT_BITMAP)
          check_free_type_error (FT_Glyph_To_Bitmap (&glyph, FT_RENDER_MODE_NORMAL, 0, 1), "FT_Glyph_To_Bitmap");

        bitmap = &((FT_BitmapGlyph)glyph)->bitmap;
      }
      else
        bitmap = &face->glyph->bitmap;

      for (int j = 0; j < bitmap->rows; j++)
      {
        unsigned char* buffer = bitmap->buffer + bitmap->pitch * j;
        unsigned char* destination_pixel = &image_data[((origin_y + (bitmap->rows - j - 1)) * row_size) + origin_x * bytes_per_pixel];

        for (int k = 0; k < bitmap->width; k++, destination_pixel += bytes_per_pixel, buffer++)
          memset (destination_pixel, *buffer, bytes_per_pixel);
      }

      FT_Done_Glyph (glyph);
    }

    FT_Stroker_Done (stroker);

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
  catch (xtl::exception& e)
  {
    e.touch ("media::convert (const FontDesc&, Font&, Image&)");
    throw;
  }
}

}
