#include "shared.h"

using namespace media;
using namespace media::freetype;

namespace
{

const char* LOG_NAME = "media::freetype::FreetypeLibrary";

}

namespace media
{

namespace freetype
{

/*
   Библиотека freetype
*/

struct FreetypeLibrary::Impl : public common::Lockable, public xtl::reference_counter
{
  FT_Library library; //freetype библиотека, создаем каждый раз новую для возможной работы со шрифтами в разных нитях

  ///Конструктор / деструктор
  Impl ()
    : library (0)
  {
    try
    {
      check_free_type_error (FT_Init_FreeType (&library), "::FT_Init_FreeType");
    }
    catch (xtl::exception& e)
    {
      Cleanup ();
      e.touch ("media::freetype::FreetypeLibrary::FreetypeLibrary");
      throw;
    }
    catch (...)
    {
      Cleanup ();
      throw;
    }
  }

  ~Impl ()
  {
    Cleanup ();
  }

  void Cleanup ()
  {
    if (library)
    {
      FT_Error result = FT_Done_FreeType (library);

      if (!result)
        common::Log (LOG_NAME).Printf ("Can't destroy freetype library, error '%s'", get_free_type_error_name (result));
    }
  }
};

}

}

/*
   Конструктор / деструктор / копирование
*/

FreetypeLibrary::FreetypeLibrary ()
  : impl (new Impl)
  {}

FreetypeLibrary::FreetypeLibrary (const FreetypeLibrary& source)
  : impl (source.impl)
{
  addref (impl);
}

FreetypeLibrary::~FreetypeLibrary ()
{
  release (impl);
}

FreetypeLibrary& FreetypeLibrary::operator = (const FreetypeLibrary& source)
{
  FreetypeLibrary (source).Swap (*this);

  return *this;
}

/*
   Freetype API
*/

void FreetypeLibrary::FT_Done_Face (FT_Face face)
{
  common::Lock lock (*impl);

  check_free_type_error (::FT_Done_Face (face), "::FT_Done_Face");
}

void FreetypeLibrary::FT_Done_Glyph (FT_Glyph glyph)
{
  common::Lock lock (*impl);

  ::FT_Done_Glyph (glyph);
}

FT_UInt FreetypeLibrary::FT_Get_Char_Index (FT_Face face, FT_ULong charcode)
{
  common::Lock lock (*impl);

  return ::FT_Get_Char_Index (face, charcode);
}

void FreetypeLibrary::FT_Get_Glyph (FT_GlyphSlot slot, FT_Glyph *aglyph)
{
  common::Lock lock (*impl);

  check_free_type_error (::FT_Get_Glyph (slot, aglyph), "::FT_Get_Glyph");
}

bool FreetypeLibrary::FT_Get_Kerning (FT_Face face, FT_UInt left_glyph, FT_UInt right_glyph, FT_UInt kern_mode, FT_Vector *akerning, bool nothrow)
{
  common::Lock lock (*impl);

  FT_Error result = ::FT_Get_Kerning (face, left_glyph, right_glyph, kern_mode, akerning);

  if (nothrow)
    return result == 0;

  check_free_type_error (result, "::FT_Get_Kerning");

  return true;
}

void FreetypeLibrary::FT_Glyph_Stroke (FT_Glyph *pglyph, FT_Stroker stroker, FT_Bool destroy)
{
  common::Lock lock (*impl);

  check_free_type_error (::FT_Glyph_Stroke (pglyph, stroker, destroy), "::FT_Glyph_Stroke");
}

void FreetypeLibrary::FT_Glyph_To_Bitmap (FT_Glyph* the_glyph, FT_Render_Mode render_mode, FT_Vector* origin, FT_Bool destroy)
{
  common::Lock lock (*impl);

  check_free_type_error (::FT_Glyph_To_Bitmap (the_glyph, render_mode, origin, destroy), "::FT_Glyph_To_Bitmap");
}

bool FreetypeLibrary::FT_Load_Char (FT_Face face, FT_ULong charcode, FT_Int32 load_flags, bool nothrow)
{
  common::Lock lock (*impl);

  FT_Error result = ::FT_Load_Char (face, charcode, load_flags);

  if (nothrow)
    return result == 0;

  check_free_type_error (result, "::FT_Load_Char");

  return true;
}

void FreetypeLibrary::FT_New_Memory_Face (const FT_Byte* file_base, FT_Long file_size, FT_Long face_index, FT_Face *aface)
{
  common::Lock lock (*impl);

  check_free_type_error (::FT_New_Memory_Face (impl->library, file_base, file_size, face_index, aface), "::FT_New_Memory_Face");
}

void FreetypeLibrary::FT_Select_Charmap (FT_Face face, FT_Encoding  encoding)
{
  common::Lock lock (*impl);

  check_free_type_error (::FT_Select_Charmap (face, encoding), "::FT_Select_Charmap");
}

void FreetypeLibrary::FT_Select_Size (FT_Face face, FT_Int strike_index)
{
  common::Lock lock (*impl);

  check_free_type_error (::FT_Select_Size (face, strike_index), "::FT_Select_Size");
}

void FreetypeLibrary::FT_Set_Char_Size (FT_Face face, FT_F26Dot6 char_width, FT_F26Dot6 char_height, FT_UInt horz_resolution, FT_UInt vert_resolution)
{
  common::Lock lock (*impl);

  check_free_type_error (::FT_Set_Char_Size (face, char_width, char_height, horz_resolution, vert_resolution), "::FT_Set_Char_Size");
}

void FreetypeLibrary::FT_Stroker_Done (FT_Stroker stroker)
{
  common::Lock lock (*impl);

  ::FT_Stroker_Done (stroker);
}

void FreetypeLibrary::FT_Stroker_New (FT_Stroker *astroker)
{
  common::Lock lock (*impl);

  check_free_type_error (::FT_Stroker_New (impl->library, astroker), "::FT_Stroker_New");
}

void FreetypeLibrary::FT_Stroker_Set (FT_Stroker stroker, FT_Fixed radius, FT_Stroker_LineCap line_cap, FT_Stroker_LineJoin line_join, FT_Fixed miter_limit)
{
  common::Lock lock (*impl);

  ::FT_Stroker_Set (stroker, radius, line_cap, line_join, miter_limit);
}

/*
   Обмен
*/

void FreetypeLibrary::Swap (FreetypeLibrary& source)
{
  stl::swap (impl, source.impl);
}
