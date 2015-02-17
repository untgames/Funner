#include "shared.h"

using namespace media;
using namespace media::freetype;

namespace
{

const char* LOG_NAME = "media.freetype.FreetypeFace";

}

namespace media
{

namespace freetype
{

/*
   Шрифт
*/

struct FreetypeFace::Impl
{
  DataBufferPtr   data;                    //данные файла шрифта
  FreetypeLibrary library;                 //библиотека
  FT_Face         face;                    //шрифт
  unsigned int    current_size;            //текущий установленный размер шрифта
  unsigned int    current_horizontal_dpi;  //текущее установленное разрешение целевого устройства вывода
  unsigned int    current_vertical_dpi;    //текущее установленное разрешение целевого устройства вывода

  ///Конструктор / деструктор
  Impl (const DataBufferPtr& in_data, const FreetypeLibrary& in_library, unsigned int face_index)
    : data (in_data)
    , library (in_library)
    , face (0)
    , current_size (0)
    , current_horizontal_dpi (0)
    , current_vertical_dpi (0)
  {
    library.FT_New_Memory_Face ((const FT_Byte*)data->data (), (FT_Long)data->size (), face_index, &face);
    library.FT_Select_Charmap (face, FT_ENCODING_UNICODE);
  }

  ~Impl ()
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

  ///Получение ближайшего доступного размера шрифта, если такой не найден - возвращает 0
  unsigned int GetNearestFontSize (unsigned int size, unsigned int size_eps)
  {
    if (face->face_flags & FT_FACE_FLAG_SCALABLE)
      return size;

    if (!face->num_fixed_sizes)
      return 0;

    unsigned int min_size = size > size_eps ? size - size_eps : 1;
    unsigned int max_size = size + size_eps;

    if (max_size < size)
      max_size = (unsigned int)-1;

    unsigned int best_size;
    unsigned int size_diff  = (unsigned int)-1;
    bool         size_found = false;

    for (int i = 0; i < face->num_fixed_sizes; i++)
    {
      unsigned int current_size = face->available_sizes [i].width;

      if (current_size >= min_size && current_size <= max_size)
      {
        size_found = true;

        unsigned int current_size_diff = size > current_size ? size - current_size : current_size - size;

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

  ///Установка размера шрифта
  void SetSize (unsigned int size, unsigned int horizontal_dpi, unsigned int vertical_dpi)
  {
    if (size == current_size && horizontal_dpi == current_horizontal_dpi && vertical_dpi == current_vertical_dpi)
      return;

    for (int i = 0; i < face->num_fixed_sizes; i++)
    {
      unsigned int current_fixed_size = face->available_sizes [i].width;

      if (size == current_fixed_size)
      {
        library.FT_Select_Size (face, i);

        current_size           = size;
        current_horizontal_dpi = horizontal_dpi;
        current_vertical_dpi   = vertical_dpi;

        return;
      }
    }

    library.FT_Set_Char_Size (face, (FT_F26Dot6)((size << 6) * (72.f / horizontal_dpi)), 0, horizontal_dpi, vertical_dpi);

    current_size           = size;
    current_horizontal_dpi = horizontal_dpi;
    current_vertical_dpi   = vertical_dpi;
  }
};

}

}

/*
   Конструктор / деструктор
*/

FreetypeFace::FreetypeFace (const DataBufferPtr& data, const FreetypeLibrary& library, unsigned int face_index)
  : impl (new Impl (data, library, face_index))
  {}

FreetypeFace::~FreetypeFace ()
{
  delete impl;
}

/*
   FreeType-дескриптор шрифта
*/

FT_Face FreetypeFace::FaceHandle ()
{
  return impl->face;
}

/*
   Получение ближайшего доступного размера шрифта, если такой не найден - возвращает 0
*/
unsigned int FreetypeFace::GetNearestFontSize (unsigned int size, unsigned int size_eps)
{
  return impl->GetNearestFontSize (size, size_eps);
}

/*
   Установка размера шрифта
*/

void FreetypeFace::SetSize (unsigned int size, unsigned int horizontal_dpi, unsigned int vertical_dpi)
{
  impl->SetSize (size, horizontal_dpi, vertical_dpi);
}
