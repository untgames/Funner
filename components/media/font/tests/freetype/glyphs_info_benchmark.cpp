#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/uninitialized_storage.h>

#include <common/file.h>
#include <common/time.h>

#include <ft2build.h>
#include <freetype/freetype.h>

const char*  FILE_NAME                  = "data/times_new_roman.ttf";
const size_t FIRST_GLYPH                = 50;
const size_t GLYPHS_COUNT               = 1000;
const size_t ADVANCES_ITERATIONS_COUNT  = 1000;
const size_t RASTERIZE_ITERATIONS_COUNT = 100;
const size_t KERNINGS_ITERATIONS_COUNT  = 10;

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

int main ()
{
  printf ("Results of glyphs_info_benchmark:\n");

  try
  {
    FT_Library library;

    if (FT_Init_FreeType (&library))
      throw xtl::format_operation_exception ("::FT_Init_FreeType", "Can't init freetype library\n");

    common::InputFile font_file (FILE_NAME);

    xtl::uninitialized_storage<char> font_data (font_file.Size ());

    font_file.Read (font_data.data (), font_data.size ());

    FT_Face face;

    if (FT_New_Memory_Face (library, (const FT_Byte*)font_data.data (), font_data.size (), 0, &face))
      throw xtl::format_operation_exception ("FontFace::FontFace", "Can't create font face\n");

    check_free_type_error (FT_Select_Charmap (face, FT_ENCODING_UNICODE), "::FT_Select_Charmap");

    size_t freetype_char_size = 32 * 64;

    check_free_type_error (FT_Set_Char_Size (face, freetype_char_size, freetype_char_size, 72, 72), "::FT_Set_Char_Size");

    xtl::uninitialized_storage<FT_UInt> ft_char_indices (GLYPHS_COUNT);

    for (size_t i = FIRST_GLYPH; i < FIRST_GLYPH + GLYPHS_COUNT; i++)
      ft_char_indices.data () [i] = FT_Get_Char_Index (face, i);

    {
      size_t start_time = common::milliseconds ();

      for (size_t i = 0; i < ADVANCES_ITERATIONS_COUNT; i++)
      {
        for (size_t j = FIRST_GLYPH; j < FIRST_GLYPH + GLYPHS_COUNT; j++)
          FT_Load_Char (face, j, FT_LOAD_DEFAULT);
      }

      printf ("Get glyphs sizes speed is %f/s\n", ADVANCES_ITERATIONS_COUNT / ((common::milliseconds () - start_time) / 1000.f));
    }

    {
      size_t start_time = common::milliseconds ();

      for (size_t i = 0; i < RASTERIZE_ITERATIONS_COUNT; i++)
      {
        for (size_t j = FIRST_GLYPH; j < FIRST_GLYPH + GLYPHS_COUNT; j++)
          FT_Load_Char (face, j, FT_LOAD_RENDER);
      }

      printf ("Rasterize glyphs speed is %f/s\n", RASTERIZE_ITERATIONS_COUNT / ((common::milliseconds () - start_time) / 1000.f));
    }

    {
      size_t start_time = common::milliseconds ();

      for (size_t i = 0; i < KERNINGS_ITERATIONS_COUNT; i++)
      {
        for (size_t j = 0; j < GLYPHS_COUNT; j++)
          for (size_t k = 0; k < GLYPHS_COUNT; k++)
          {
            FT_Vector kerning;

            FT_Get_Kerning (face, ft_char_indices.data () [j], ft_char_indices.data () [k], FT_KERNING_UNFITTED, &kerning);
          }
      }

      printf ("Get kernings speed is %f/s\n", KERNINGS_ITERATIONS_COUNT / ((common::milliseconds () - start_time) / 1000.f));
    }

    FT_Done_Face (face);

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
