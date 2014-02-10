#include "shared.h"

using namespace media;
using namespace media::freetype;

namespace
{

const char* LOG_NAME = "media::freetype::FreetypeFontDesc";

//Шрифт free type
class FreeTypeFace : public xtl::reference_counter
{
  public: //TODO hold library and memory, threading protection
    FreeTypeFace (FT_Library library, const FT_Byte* data, size_t data_size, size_t face_index)
      : face (0)
    {
      check_free_type_error (FT_New_Memory_Face (library, data, data_size, face_index, &face), "::FT_New_Memory_Face");
    }

    ~FreeTypeFace ()
    {
      if (face)
      {
        FT_Error result = FT_Done_Face (face);

        if (!result)
          common::Log (LOG_NAME).Printf ("Can't destroy freetype face, error '%s'", get_free_type_error_name (result));
      }
    }

    FT_Face FaceHandle ()
    {
      return face;
    }

  private:
    FreeTypeFace (const FreeTypeFace&);             //no impl
    FreeTypeFace& operator = (const FreeTypeFace&); //no impl

  private:
    FT_Face face;
};

typedef xtl::intrusive_ptr<FreeTypeFace> FreeTypeFacePtr;
typedef stl::vector<FreeTypeFacePtr>     FacesArray;

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
  xtl::uninitialized_storage<char> font_data; //данные файла шрифта
  FT_Library                       library;   //freetype библиотека, создаем каждый раз новую для возможной работы со шрифтами в разных нитях
  FacesArray                       faces;     //шрифты

  ///Конструктор / деструктор
  Impl (const char* file_name)
    : library (0)
  {
    try
    {
      common::InputFile font_file (file_name);

      font_data.resize (font_file.Size (), false);

      font_file.Read (font_data.data (), font_data.size ());

      check_free_type_error (FT_Init_FreeType (&library), "::FT_Init_FreeType");

      FT_Face test_face;

      check_free_type_error (FT_New_Memory_Face (library, (const FT_Byte*)font_data.data (), font_data.size (), -1, &test_face), "::FT_New_Memory_Face");

      FT_Long faces_count = test_face->num_faces;

      check_free_type_error (FT_Done_Face (test_face), "::FT_Done_Face");

      faces.reserve (faces_count);

      for (size_t i = 0; i < faces_count; i++)
        faces.push_back (FreeTypeFacePtr (new FreeTypeFace (library, (const FT_Byte*)font_data.data (), font_data.size (), i), false));
    }
    catch (xtl::exception& e)
    {
      Cleanup ();
      e.touch ("media::freetype::FreetypeFontDesc::FreetypeFontDesc");
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
    faces.clear ();

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
  if (index > impl->faces.size ())
    throw xtl::make_range_exception ("media::freetype::FreetypeFontDesc::CreateFont", "index", index, 0u, impl->faces.size ());

  //TODO
}

bool FreetypeFontDesc::CanCreateFont (size_t index, const FontCreationParams& params)
{
  if (index > impl->faces.size ())
    throw xtl::make_range_exception ("media::freetype::FreetypeFontDesc::CanCreateFont", "index", index, 0u, impl->faces.size ());

  //TODO
}
