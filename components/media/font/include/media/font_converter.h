#ifndef MEDIALIB_FONT_CONVERTER_HEADER
#define MEDIALIB_FONT_CONVERTER_HEADER

#include <media/font.h>

namespace media
{

//forward declaration
class Image;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание шрифта
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FontDesc
{
  const char*    file_name;        //имя файла со шрифтом
  const size_t*  char_codes;       //строка с символами для генерации
  size_t         char_codes_count; //количество символов для генерации
  size_t         glyph_size;       //максимальный размер стороны символа
  float          stroke_width;     //ширина линии обводки
  size_t         first_glyph_code; //код первого глифа
  size_t         glyph_interval;   //пропуск между соседними глифами
  bool           fast_convert;     //быстрая генерация (менее качественная)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конвертация шрифта
///////////////////////////////////////////////////////////////////////////////////////////////////
void convert (const FontDesc& font_desc, Font& result_font, Image& result_image);

}

#endif
