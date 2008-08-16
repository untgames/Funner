#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации TextLine
*/

struct TextLine::Impl
{
  stl::string       text;
  bool              text_need_update;
  stl::wstring      text_unicode;
  bool              text_unicode_need_update;
  stl::string       font_name;
  vec4f             color;
  TextLineAlignment horizontal_alignment;
  TextLineAlignment vertical_alignment;

  Impl () : text_need_update (false), text_unicode_need_update (false), color (1.f, 1.f, 1.f, 1.f), 
            horizontal_alignment (TextLineAlignment_Left), vertical_alignment (TextLineAlignment_Top) {}
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
   Установка/получение текста
*/

void TextLine::SetText (const char* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetText", "text");

  impl->text = text;

  impl->text_need_update = false;
  impl->text_unicode_need_update = true;

  UpdateNotify ();
}

void TextLine::SetText (const wchar_t* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetText", "text");

  impl->text_unicode = text;

  impl->text_unicode_need_update = false;
  impl->text_need_update = true;

  UpdateNotify ();
}

const char* TextLine::Text () const
{
  if (impl->text_need_update)
  {
    impl->text = common::tostring (impl->text_unicode);
    impl->text_need_update = false;
  }

  return impl->text.c_str ();
}

const wchar_t* TextLine::TextUnicode () const
{
  if (impl->text_unicode_need_update)
  {
    impl->text_unicode = common::towstring (impl->text);
    impl->text_unicode_need_update = false;
  }

  return impl->text_unicode.c_str ();
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
