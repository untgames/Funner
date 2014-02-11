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

/*
   Обмен
*/

void FreetypeLibrary::Swap (FreetypeLibrary& source)
{
  stl::swap (impl, source.impl);
}
