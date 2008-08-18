#include <cmath>

#include <xtl/common_exceptions.h>
#include <xtl/uninitialized_storage.h>

#include <common/file.h>
#include <common/log.h>

#include <media/font_converter.h>
#include <media/image.h>

#include <ft2build.h>
#include <freetype/freetype.h>

namespace
{

size_t RESOLUTION = 72;

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

  for (int i=1; i < sizeof (size_t) * 8; i *= 2)
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

  if (!font_desc.char_codes_line)
    throw xtl::make_null_argument_exception (METHOD_NAME, "font_desc.char_codes_line");
  
  size_t char_codes_count = wcslen (font_desc.char_codes_line);

  if (!char_codes_count)
    throw xtl::format_operation_exception (METHOD_NAME, "Empty char codes string");
  
  if (!font_desc.glyph_size)
    throw xtl::make_null_argument_exception (METHOD_NAME, "font_desc.glyph_size");

  FreeTypeLibrary freetype_library;

  FontFace font_face (font_desc.file_name, freetype_library.Library ());

  FT_Face& face = font_face.Face ();

  size_t freetype_char_size = font_desc.glyph_size * 64;

  if (FT_Set_Char_Size (face, freetype_char_size, freetype_char_size, RESOLUTION, RESOLUTION))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set char size");

    // Calculate maximum width, height and bearing

  int    max_height = 0, max_width = 0, max_bearing = 0;
  size_t glyphs_count = 0;

  for (size_t i = 0; i < char_codes_count; i++)
  {
    if (FT_Load_Char (face, font_desc.char_codes_line [i], FT_LOAD_RENDER))
    {
      common::LogSystem::Printf (LOG_NAME, "Can't load char %u.", i);
    
      continue;
    }

    if ((2 * (face->glyph->bitmap.rows << 6) - face->glyph->metrics.horiBearingY) > max_height)
      max_height = (2 * (face->glyph->bitmap.rows << 6) - face->glyph->metrics.horiBearingY);
    
    if (face->glyph->metrics.horiBearingY > max_bearing)
      max_bearing = face->glyph->metrics.horiBearingY;

    if ((face->glyph->advance.x >> 6) + (face->glyph->metrics.horiBearingX >> 6) > max_width)
      max_width = (face->glyph->advance.x >> 6) + (face->glyph->metrics.horiBearingX >> 6);

    glyphs_count++;
  }

    // Now work out how big our texture needs to be

  size_t raw_size = (max_width + font_desc.glyph_interval) * ((max_height >> 6) + font_desc.glyph_interval) * glyphs_count;

  size_t texture_side = (size_t)sqrt ((float)raw_size);

    // just in case the size might chop a glyph in half, add another glyph width/height
  
  texture_side += stl::max (max_width, (max_height >> 6));

    // Now round up to nearest power of two
  
  texture_side = get_next_higher_power_of_two (texture_side);

    // Would we benefit from using a non-square texture (2X width)
  
  size_t final_width, final_height;

  if (texture_side * texture_side * 0.5 >= raw_size)
    final_height = (size_t)((float)texture_side * 0.5f);
  else
    final_height = texture_side;

  final_width = texture_side;

  const size_t pixel_bytes = get_bytes_per_pixel (result_image.Format ());
  
  size_t data_width = final_width * pixel_bytes;
  size_t data_size  = final_width * final_height * pixel_bytes;

  media::Font font;

  font.Rename (result_font.Name ());
  font.SetImageName (result_image.Name ());
  font.ResizeGlyphsTable (char_codes_count);
  font.SetFirstGlyphCode (font_desc.first_glyph_code);

  media::Image image (final_width, final_height, 1, result_image.Format ());

  image.Rename (result_image.Name ());

  unsigned char* image_data = (unsigned char*)image.Bitmap ();

    // Reset content (Black, transparent)
  
  memset (image_data, 0, data_size);

  size_t column = 0, current_row = 0;

  media::GlyphInfo *current_glyph = font.Glyphs ();

  for (size_t i = 0; i < char_codes_count; i++, current_glyph++)
  {
      // Load & render glyph
      
    if (FT_Load_Char (face, font_desc.char_codes_line [i], FT_LOAD_RENDER))
    {
        // problem loading this glyph, continue
      set_null_glyph_data (current_glyph);

      continue;
    }

    FT_Int advance = (face->glyph->advance.x >> 6) + (face->glyph->metrics.horiBearingX >> 6);

    unsigned char* buffer = face->glyph->bitmap.buffer;

    if (!buffer)
    {
      common::LogSystem::Printf (LOG_NAME, "Freetype returned null for character %u.", i);
      
      set_null_glyph_data (current_glyph);
      current_glyph->advance_x = face->glyph->metrics.horiAdvance >> 6;

      continue;
    }

    int y_bearing = (max_bearing >> 6) - (face->glyph->metrics.horiBearingY >> 6);

    for (int j = 0; j < face->glyph->bitmap.rows; j++)
    {
      size_t row = current_row + y_bearing + face->glyph->bitmap.rows - j - 1;

      unsigned char* destination_pixel = &image_data[(row * data_width) + column * pixel_bytes];

      for (int k = 0; k < face->glyph->bitmap.width; k++, destination_pixel += pixel_bytes, buffer++)
        memset (destination_pixel, *buffer, pixel_bytes);
    }

    current_glyph->x_pos     = column;
    current_glyph->y_pos     = current_row + y_bearing;
    current_glyph->width     = face->glyph->metrics.width >> 6;
    current_glyph->height    = face->glyph->metrics.height >> 6;
    current_glyph->bearing_x = face->glyph->metrics.horiBearingX >> 6;
    current_glyph->bearing_y = face->glyph->metrics.horiBearingY >> 6;
    current_glyph->advance_x = face->glyph->metrics.horiAdvance >> 6;
    current_glyph->advance_y = 0;

      // Advance a column
    column += (advance + font_desc.glyph_interval);

      // If at end of row
    if ((final_width - 1) < (column + advance))
    {
      current_row += (max_height >> 6) + font_desc.glyph_interval;
      column = 0;
    }
  }

  for (size_t i = 0; i < char_codes_count; i++)
    for (size_t j = 0; j < char_codes_count; j++)
    {
      FT_Vector kerning;

      if (FT_Get_Kerning (face, FT_Get_Char_Index (face, font_desc.char_codes_line [i]), FT_Get_Char_Index (face, font_desc.char_codes_line [j]), FT_KERNING_UNFITTED, &kerning))
      {
        common::LogSystem::Printf (LOG_NAME, "Can't get kerning for pair %u-%u.", i, j);
        continue;
      }

      if (kerning.x || kerning.y)
      {
        media::KerningInfo kerning_info;

        kerning_info.x_kerning = (float)kerning.x / 64.f;
        kerning_info.y_kerning = (float)kerning.y / 64.f;

        font.InsertKerning (i, j, kerning_info);
      }
    }

  swap (result_image, image);
  swap (result_font,  font);
}

}
