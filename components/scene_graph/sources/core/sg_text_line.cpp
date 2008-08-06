#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации TextLine
*/

struct TextLine::Impl
{
  stl::string text;
  stl::string font_name;
  vec4f       color;

  Impl () : color (1.f, 1.f, 1.f, 1.f) {}
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

  UpdateNotify ();
}

const char* TextLine::Text () const
{
  return impl->text.c_str ();
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
    Метод, вызываемый при посещении объекта
*/

void TextLine::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
