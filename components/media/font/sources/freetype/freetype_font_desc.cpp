#include "shared.h"

using namespace media;
using namespace media::freetype;

namespace
{

const char* LOG_NAME = "media::freetype::FreetypeFontDesc";

typedef xtl::uninitialized_storage<char> DataBuffer;
typedef xtl::shared_ptr<DataBuffer>      DataBufferPtr;

//Шрифт free type
class FreeTypeFace : public xtl::reference_counter
{
  public:
    FreeTypeFace (const DataBufferPtr& in_data, const FreetypeLibrary& in_library, size_t face_index)
      : data (in_data)
      , library (in_library)
      , face (0)
    {
      library.FT_New_Memory_Face ((const FT_Byte*)data->data (), data->size (), face_index, &face);
    }

    ~FreeTypeFace ()
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

  private:
    FreeTypeFace (const FreeTypeFace&);             //no impl
    FreeTypeFace& operator = (const FreeTypeFace&); //no impl

  private:
    DataBufferPtr   data;     //данные файла шрифта
    FreetypeLibrary library;  //библиотека
    FT_Face         face;     //шрифт
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
  DataBufferPtr   font_data; //данные файла шрифта
  FreetypeLibrary library;   //freetype библиотека, создаем каждый раз новую для возможной работы со шрифтами в разных нитях
  FacesArray      faces;     //шрифты

  ///Конструктор / деструктор
  Impl (const char* file_name)
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
        faces.push_back (FreeTypeFacePtr (new FreeTypeFace (font_data, library, i), false));
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
