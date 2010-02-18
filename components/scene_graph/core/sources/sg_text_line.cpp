#include "shared.h"

using namespace scene_graph;
using namespace math;

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

}


/*
    Описание реализации TextLine
*/

struct TextLine::Impl
{
  typedef stl::vector <math::vec4f> CharsColors;

  stl::string       text;
  stl::wstring      text_unicode;
  size_t            text_hash;
  size_t            text_unicode_hash;
  bool              text_need_update;
  bool              text_unicode_need_update;
  stl::string       font_name;
  vec4f             color;
  vec4f             last_asked_char_color;    //переменная для хранения возвращаемого значения
  TextLineAlignment horizontal_alignment;
  TextLineAlignment vertical_alignment;
  CharsColors       chars_colors_factors;     //множители цвета букв
  CharsColors       chars_colors;
  bool              chars_colors_need_update;

  Impl ()
   : text_hash (common::strhash ("")),
     text_unicode_hash (common::strhash (L"")),
     text_need_update (false),
     text_unicode_need_update (false),
     color (1.f, 1.f, 1.f, 1.f), 
     horizontal_alignment (TextLineAlignment_Left),
     vertical_alignment (TextLineAlignment_Top),
     chars_colors_need_update (false)
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

  if (first >= text_length)
    throw xtl::make_range_exception (METHOD_NAME, "first", first, 0u, text_length);

  if (end > text_length)
    throw xtl::make_range_exception (METHOD_NAME, "first + count", end, 0u, text_length + 1);

  if (impl->chars_colors_factors.empty ())
    impl->chars_colors_factors.resize (TextLength (), 1);

  stl::fill (impl->chars_colors_factors.begin () + first, impl->chars_colors_factors.begin () + end, color);

  impl->chars_colors_need_update = true;

  UpdateNotify ();
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

  if (first >= text_length)
    throw xtl::make_range_exception (METHOD_NAME, "first", first, 0u, text_length);

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

void TextLine::SetText (const char* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetText", "text");

  impl->text                     = text;
  impl->text_hash                = common::strhash (impl->text.c_str ());  
  impl->text_need_update         = false;
  impl->text_unicode_need_update = true;

  impl->OnTextChanged ();

  UpdateNotify ();
}

void TextLine::SetText (const wchar_t* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetText", "text");

  impl->text_unicode             = text;
  impl->text_unicode_hash        = common::strhash (impl->text_unicode.c_str ());
  impl->text_unicode_need_update = false;
  impl->text_need_update         = true;

  impl->OnTextChanged ();

  UpdateNotify ();
}

const char* TextLine::Text () const
{
  if (impl->text_need_update)
  {
    impl->text             = common::tostring (impl->text_unicode);
    impl->text_hash        = common::strhash (impl->text.c_str ());
    impl->text_need_update = false;
  }

  return impl->text.c_str ();
}

const wchar_t* TextLine::TextUnicode () const
{
  if (impl->text_unicode_need_update)
  {
    impl->text_unicode             = common::towstring (impl->text);
    impl->text_unicode_hash        = common::strhash (impl->text_unicode.c_str ());    
    impl->text_unicode_need_update = false;
  }

  return impl->text_unicode.c_str ();
}

size_t TextLine::TextLength  () const
{
  if (!impl->text_need_update)
    return impl->text.length ();
  else if (!impl->text_need_update)
    return impl->text_unicode.length ();
  else
    return xtl::xstrlen (Text ());
}

/*
    Хэш текста
*/

size_t TextLine::TextHash () const
{
  if (impl->text_need_update)
    Text (); //обновление хэша

  return impl->text_hash;
}

size_t TextLine::TextUnicodeHash () const
{
  if (impl->text_unicode_need_update)
    TextUnicode (); //обновление хэша

  return impl->text_unicode_hash;
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
    Entity::AcceptCore (visitor);
}
