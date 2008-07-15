#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации Sprite
*/

struct Sprite::Impl
{
  stl::string material; //имя материала
  vec4f       color;    //цвет объекта
  size_t      frame;    //номер кадра
};

/*
    Конструктор / деструктор
*/

Sprite::Sprite ()
  : impl (new Impl)
{
}

Sprite::~Sprite ()
{
}

/*
    Создание спрайта
*/

Sprite::Pointer Sprite::Create ()
{
  return Pointer (new Sprite, false);
}

/*
    Материал
*/

void Sprite::SetMaterial (const char* in_material)
{
  if (!in_material)
    throw xtl::make_null_argument_exception ("scene_graph::Sprite::SetMaterial", "material");

  impl->material = in_material;
  
  UpdateNotify ();
}

const char* Sprite::Material () const
{
  return impl->material.c_str ();
}

/*
   Установка номера кадра
*/

void Sprite::SetFrame (size_t frame)
{
  impl->frame = frame;
  
  UpdateNotify ();
}

size_t Sprite::Frame () const
{
  return impl->frame;
}

/*
    Цвет спрайта
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

void Sprite::SetColor (const vec4f& color)
{
  impl->color = clamp (color);

  UpdateNotify ();
}

void Sprite::SetColor (float red, float green, float blue, float alpha)
{
  SetColor (vec4f (red, green, blue, alpha));
}

void Sprite::SetColor (float red, float green, float blue)
{
  SetColor (vec4f (red, green, blue, impl->color.w));
}

void Sprite::SetAlpha (float alpha)
{
  impl->color.w = clamp (alpha);

  UpdateNotify ();
}

const vec4f& Sprite::Color () const
{
  return impl->color;
}

float Sprite::Alpha () const
{
  return impl->color.w;
}

/*
    Метод, вызываемый при посещении объекта
*/

void Sprite::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
