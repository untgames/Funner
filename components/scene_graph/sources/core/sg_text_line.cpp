#include "shared.h"

using namespace scene_graph;
using namespace math;

#ifdef __BIG_ENDIAN__
const common::Encoding UTF32_ENCODING = common::Encoding_UTF32BE;
#else
const common::Encoding UTF32_ENCODING = common::Encoding_UTF32LE;
#endif

namespace
{

float clamp (float x)
{
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;

  return x;
}

vec4f clamp (const vec4f& color)
{
  return vec4f (clamp (color.x), clamp (color.y), clamp (color.z), clamp (color.w));
}

const math::vec4f DEFAULT_CHAR_COLOR_FACTOR = 1.f;

}


/*
    Описание реализации TextLine
*/

typedef xtl::uninitialized_storage<unsigned int> Utf32Buffer;

struct TextLine::Impl: public xtl::instance_counter<TextLine>
{
  typedef stl::vector <math::vec4f> CharsColors;

  stl::string       text_utf8;
  Utf32Buffer       text_utf32;
  size_t            length;
  size_t            text_utf8_hash;
  size_t            text_utf32_hash;
  bool              text_utf8_need_update;
  bool              text_utf32_need_update;
  stl::string       font_name;
  vec4f             color;
  vec4f             last_asked_char_color;    //переменная для хранения возвращаемого значения
  TextLineAlignment horizontal_alignment;
  TextLineAlignment vertical_alignment;
  CharsColors       chars_colors_factors;     //множители цвета букв
  CharsColors       chars_colors;
  bool              chars_colors_need_update;
  float             spacing_multiplier;

  Impl ()
   : length (0),
     text_utf8_hash (common::strhash ("")),
     text_utf32_hash (common::strhash (L"")),     
     text_utf8_need_update (false),
     text_utf32_need_update (false),
     color (1.f, 1.f, 1.f, 1.f), 
     horizontal_alignment (TextLineAlignment_Left),
     vertical_alignment (TextLineAlignment_Top),
     chars_colors_need_update (false),
     spacing_multiplier (1.f)
   {}

  void OnTextChanged ()
  {
    chars_colors_factors.clear ();
    chars_colors.clear ();
  }

  void UpdateCharsColors ()
  {
    size_t chars_count = chars_colors_factors.size ();

    chars_colors.resize (chars_count);

    for (size_t i = 0; i < chars_count; i++)
      chars_colors [i] = clamp (color * chars_colors_factors [i]);

    chars_colors_need_update = true;
  }
};

/*
    Конструктор / деструктор
*/

TextLine::TextLine ()
  : impl (new Impl)
{
}

TextLine::~TextLine ()
{
}

/*
    Создание линии текста
*/

TextLine::Pointer TextLine::Create ()
{
  return Pointer (new TextLine, false);
}

/*
    Цвет текста
*/

void TextLine::SetColor (const vec4f& color)
{
  impl->color = clamp (color);

  UpdateNotify ();
}

void TextLine::SetColor (float red, float green, float blue, float alpha)
{
  SetColor (vec4f (red, green, blue, alpha));
}

void TextLine::SetColor (float red, float green, float blue)
{
  SetColor (vec4f (red, green, blue, impl->color.w));
}

const vec4f& TextLine::Color () const
{
  return impl->color;
}

/*
   Цвет определенных символов текста
*/

void TextLine::SetCharsColorFactors (size_t first, size_t count, const math::vec4f& color)
{
  static const char* METHOD_NAME = "scene_graph::TextLine::SetCharsColor";

  size_t text_length = TextLength (),
         end         = first + count;

  if (end > text_length)
    throw xtl::make_range_exception (METHOD_NAME, "first + count", end, 0u, text_length + 1);

  if (impl->chars_colors_factors.empty ())
    impl->chars_colors_factors.resize (TextLength (), DEFAULT_CHAR_COLOR_FACTOR);

  stl::fill (impl->chars_colors_factors.begin () + first, impl->chars_colors_factors.begin () + end, color);

  impl->chars_colors_need_update = true;

  UpdateNotify ();
}

const math::vec4f& TextLine::CharColorFactor (size_t index) const
{
  static const char* METHOD_NAME = "scene_graph::TextLine::CharColorFactor";

  size_t text_length = TextLength ();

  if (index >= text_length)
    throw xtl::make_range_exception (METHOD_NAME, "index", index, 0u, text_length);

  if (impl->chars_colors_factors.empty ())
    return DEFAULT_CHAR_COLOR_FACTOR;

  return impl->chars_colors_factors [index];
}

void TextLine::CharsColorFactors (size_t first, size_t count, math::vec4f* colors) const
{
  static const char* METHOD_NAME = "scene_graph::TextLine::CharsColorFactors";

  size_t text_length = TextLength (),
         end         = first + count;

  if (end > text_length)
    throw xtl::make_range_exception (METHOD_NAME, "first + count", end, 0u, text_length + 1);

  if (impl->chars_colors_factors.empty ())
  {
    for (size_t i = 0; i < count; i++, colors++)
      *colors = 1.f;
  }
  else
    stl::copy (impl->chars_colors_factors.begin () + first, impl->chars_colors_factors.begin () + end, colors);
}

const math::vec4f& TextLine::CharColor (size_t index) const
{
  if (index >= TextLength ())
    throw xtl::make_range_exception ("scene_graph::TextLine::SetCharsColor", "index", index, 0u, TextLength ());

  if (impl->chars_colors_need_update)
    impl->UpdateCharsColors ();

  if (impl->chars_colors.empty ())
    return Color ();

  return impl->chars_colors [index];
}

void TextLine::CharsColors (size_t first, size_t count, math::vec4f* colors) const
{
  static const char* METHOD_NAME = "scene_graph::TextLine::CharsColor";

  size_t text_length = TextLength (),
         end         = first + count;

  if (end > text_length)
    throw xtl::make_range_exception (METHOD_NAME, "first + count", end, 0u, text_length + 1);

  if (impl->chars_colors_need_update)
    impl->UpdateCharsColors ();

  if (impl->chars_colors.empty ())
  {
    const math::vec4f& color = Color ();

    for (size_t i = 0; i < count; i++, colors++)
      *colors = color;
  }
  else
    stl::copy (impl->chars_colors.begin () + first, impl->chars_colors.begin () + end, colors);
}

/*
   Установка/получение текста
*/

void TextLine::SetTextUtf8 (const char* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetTextUtf8", "text");

  impl->text_utf8              = text;
  impl->text_utf8_hash         = common::strhash (impl->text_utf8.c_str ());  
  impl->length                 = impl->text_utf8.size ();
  impl->text_utf8_need_update  = false;
  impl->text_utf32_need_update = true;  

  impl->OnTextChanged ();

  UpdateNotify ();
}

void TextLine::SetTextUtf32 (const unsigned int* text, size_t length)
{  
  if (length && !text)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetText(const unsigned int*,size_t)", "text");

  impl->text_utf32.resize (length + 1, false);

  impl->length = 0;

  if (length)
  {
    size_t size = length * sizeof (unsigned int);

    memcpy (impl->text_utf32.data (), text, size);
    memset (impl->text_utf32.data () + length, 0, sizeof (unsigned int));

    impl->text_utf32_hash = common::crc32 (impl->text_utf32.data (), size);
  }
  else
  {
    memset (impl->text_utf32.data (), 0, sizeof (unsigned int));    

    impl->text_utf32_hash = ~0u;
  }

  impl->text_utf32_need_update = false;  
  impl->text_utf8_need_update  = true;
  impl->length                 = length;

  impl->OnTextChanged ();

  UpdateNotify ();
}

const char* TextLine::TextUtf8 () const
{
  if (impl->text_utf8_need_update)
  {
    try
    {
      if (!impl->text_utf32_need_update)
      {      
        if (impl->text_utf32.size () != 0)
        {
          impl->text_utf8.fast_resize (impl->length * 4);          

          const void* source           = impl->text_utf32.data ();
          size_t      source_size      = (impl->text_utf32.size () - 1) * sizeof (unsigned int);
          void*       destination      = &impl->text_utf8 [0];
          size_t      destination_size = impl->text_utf8.size ();

          convert_encoding (UTF32_ENCODING, source, source_size, common::Encoding_UTF8, destination, destination_size);

          if (source_size)
            throw xtl::format_operation_exception ("", "Internal error: buffer not enough (source_size=%u, destination_size=%u)", source_size, destination_size);

          *(char*)destination = '\0';
        }
        else
        {
          impl->text_utf8.clear ();
        }
      }
      else
      {
        throw xtl::format_operation_exception ("", "Internal error: no base text representation for conversion");
      }

      impl->text_utf8_hash        = common::strhash (impl->text_utf8.c_str ());
      impl->text_utf8_need_update = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("scene_graph::TextLine::Text");
      throw;
    }
  }

  return impl->text_utf8.c_str ();
}

const unsigned int* TextLine::TextUtf32 () const
{
  if (impl->text_utf32_need_update)
  {
    try
    {
      if (!impl->text_utf8_need_update)
      {      
        if (!impl->text_utf8.empty ())
        {
          impl->text_utf32.resize (impl->text_utf8.size () + 1, false);

          const void* source           = impl->text_utf8.c_str ();
          size_t      source_size      = impl->text_utf8.size ();
          void*       destination      = impl->text_utf32.data ();
          size_t      destination_size = impl->text_utf32.size () * sizeof (unsigned int);

          convert_encoding (common::Encoding_UTF8, source, source_size, UTF32_ENCODING, destination, destination_size);

          if (source_size || destination_size < sizeof (unsigned int))
            throw xtl::format_operation_exception ("", "Internal error: buffer not enough (source_size=%u, destination_size=%u)", source_size, destination_size);

          memset (destination, 0, sizeof (unsigned int));

          impl->length = (unsigned int*)destination - impl->text_utf32.data ();                    
        }
        else        
        {
          impl->text_utf32.resize (1, false);

          impl->length = 0;

          memset (impl->text_utf32.data (), 0, sizeof (unsigned int));
        }
      }
      else
      {
        throw xtl::format_operation_exception ("", "Internal error: no base text representation for conversion");
      }

      impl->text_utf32_hash        = impl->text_utf32.size () == 0 ? ~0u : common::crc32 (impl->text_utf32.data (), (impl->text_utf32.size () - 1) * sizeof (unsigned int));

      impl->text_utf32_need_update = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("scene_graph::TextLine::TextUtf32");
      throw;
    }
  }

  return impl->text_utf32.data ();
}

size_t TextLine::TextLength  () const
{
  if (impl->text_utf32_need_update)
    TextUtf32 (); //обновление длины

  return impl->length;
}

/*
    Хэш текста
*/

size_t TextLine::TextUtf8Hash () const
{
  if (impl->text_utf8_need_update)
    TextUtf8 (); //обновление хэша

  return impl->text_utf8_hash;
}

size_t TextLine::TextUtf32Hash () const
{
  if (impl->text_utf32_need_update)
    TextUtf32 (); //обновление хэша

  return impl->text_utf32_hash;
}

/*
   Установка/получение имени шрифта
*/

void TextLine::SetFont (const char* font_name)
{
  if (!font_name)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetFont", "font_name");

  impl->font_name = font_name;

  UpdateNotify ();
}

const char* TextLine::Font () const
{
  return impl->font_name.c_str ();
}

/*
   Установка/получение межбуквенного интервала
*/

void TextLine::SetSpacingMultiplier (float spacing_multiplier)
{
  impl->spacing_multiplier = spacing_multiplier;

  UpdateNotify ();
}

float TextLine::SpacingMultiplier () const
{
  return impl->spacing_multiplier;
}

/*
   Выравнивание
*/

void TextLine::SetAlignment (TextLineAlignment horizontal, TextLineAlignment vertical)
{
  static const char* METHOD_NAME = "scene_graph::TextLine::SetAlignment";

  if (horizontal >= TextLineAlignment_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "horizontal", horizontal, "Unknown TextLineAlignment");

  if (vertical >= TextLineAlignment_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "vertical", vertical, "Unknown TextLineAlignment");

  impl->horizontal_alignment = horizontal;
  impl->vertical_alignment = vertical;

  UpdateNotify ();
}

void TextLine::SetHorizontalAlignment (TextLineAlignment alignment)
{
  if (alignment >= TextLineAlignment_Num)
    throw xtl::make_argument_exception ("scene_graph::TextLine::SetHorizontalAlignment", "alignment", alignment, "Unknown TextLineAlignment");

  impl->horizontal_alignment = alignment;

  UpdateNotify ();
}

void TextLine::SetVerticalAlignment (TextLineAlignment alignment)
{
  if (alignment >= TextLineAlignment_Num)
    throw xtl::make_argument_exception ("scene_graph::TextLine::SetVerticalAlignment", "alignment", alignment, "Unknown TextLineAlignment");

  impl->vertical_alignment = alignment;

  UpdateNotify ();
}

TextLineAlignment TextLine::VerticalAlignment () const
{
  return impl->vertical_alignment;
}

TextLineAlignment TextLine::HorizontalAlignment () const
{
  return impl->horizontal_alignment;
}

/*
    Метод, вызываемый при посещении объекта
*/

void TextLine::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);
}

/*
    Связывание свойств
*/

void TextLine::BindProperties (common::PropertyBindingMap& bindings)
{
  VisualModel::BindProperties (bindings);

  bindings.AddProperty ("Font", xtl::bind (&TextLine::Font, this), xtl::bind (&TextLine::SetFont, this, _1));
  bindings.AddProperty ("Color", xtl::bind (&TextLine::Color, this), xtl::bind (xtl::implicit_cast<void (TextLine::*)(const math::vec4f&)> (&TextLine::SetColor), this, _1));
  bindings.AddProperty ("TextUtf8", xtl::bind (&TextLine::TextUtf8, this), xtl::bind (&TextLine::SetTextUtf8, this, _1));
}
