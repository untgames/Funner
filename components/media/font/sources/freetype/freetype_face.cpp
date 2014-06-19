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
  size_t          current_size;            //текущий установленный размер шрифта
  size_t          current_horizontal_dpi;  //текущее установленное разрешение целевого устройства вывода
  size_t          current_vertical_dpi;    //текущее установленное разрешение целевого устройства вывода

  ///Конструктор / деструктор
  Impl (const DataBufferPtr& in_data, const FreetypeLibrary& in_library, size_t face_index)
    : data (in_data)
    , library (in_library)
    , face (0)
    , current_size (0)
    , current_horizontal_dpi (0)
    , current_vertical_dpi (0)
  {
    library.FT_New_Memory_Face ((const FT_Byte*)data->data (), data->size (), face_index, &face);
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
  size_t GetNearestFontSize (size_t size, size_t size_eps)
  {
    if (face->face_flags & FT_FACE_FLAG_SCALABLE)
      return size;

    if (!face->num_fixed_sizes)
      return 0;

    size_t min_size = size > size_eps ? size - size_eps : 1;
    size_t max_size = size + size_eps;

    if (max_size < size)
      max_size = (size_t)-1;

    size_t best_size;
    size_t size_diff  = (size_t)-1;
    bool   size_found = false;

    for (size_t i = 0; i < face->num_fixed_sizes; i++)
    {
      size_t current_size = face->available_sizes [i].width;

      if (current_size >= min_size && current_size <= max_size)
      {
        size_found = true;

        size_t current_size_diff = size > current_size ? size - current_size : current_size - size;

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
  void SetSize (size_t size, size_t horizontal_dpi, size_t vertical_dpi)
  {
    if (size == current_size && horizontal_dpi == current_horizontal_dpi && vertical_dpi == current_vertical_dpi)
      return;

    for (size_t i = 0; i < face->num_fixed_sizes; i++)
    {
      size_t current_fixed_size = face->available_sizes [i].width;

      if (size == current_fixed_size)
      {
        library.FT_Select_Size (face, i);

        current_size           = size;
        current_horizontal_dpi = horizontal_dpi;
        current_vertical_dpi   = vertical_dpi;

        return;
      }
    }

    library.FT_Set_Char_Size (face, (size << 6) * (72.f / horizontal_dpi), 0, horizontal_dpi, vertical_dpi);

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

FreetypeFace::FreetypeFace (const DataBufferPtr& data, const FreetypeLibrary& library, size_t face_index)
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
size_t FreetypeFace::GetNearestFontSize (size_t size, size_t size_eps)
{
  return impl->GetNearestFontSize (size, size_eps);
}

/*
   Установка размера шрифта
*/

void FreetypeFace::SetSize (size_t size, size_t horizontal_dpi, size_t vertical_dpi)
{
  impl->SetSize (size, horizontal_dpi, vertical_dpi);
}
