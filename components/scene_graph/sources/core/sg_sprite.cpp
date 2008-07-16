#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации Sprite
*/

struct Sprite::Impl
{
  SpriteDesc sprite_desc; //описание спрайта
  
  Impl ()
  {
    sprite_desc.frame = 0;
  }
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
   Установка номера кадра
*/

void Sprite::SetFrame (size_t frame)
{
  impl->sprite_desc.frame = frame;
  
  UpdateNotify ();
}

size_t Sprite::Frame () const
{
  return impl->sprite_desc.frame;
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
  impl->sprite_desc.color = clamp (color);

  UpdateNotify ();
}

void Sprite::SetColor (float red, float green, float blue, float alpha)
{
  SetColor (vec4f (red, green, blue, alpha));
}

void Sprite::SetColor (float red, float green, float blue)
{
  SetColor (vec4f (red, green, blue, impl->sprite_desc.color.w));
}

void Sprite::SetAlpha (float alpha)
{
  impl->sprite_desc.color.w = clamp (alpha);

  UpdateNotify ();
}

const vec4f& Sprite::Color () const
{
  return impl->sprite_desc.color;
}

float Sprite::Alpha () const
{
  return impl->sprite_desc.color.w;
}

/*
    Метод, вызываемый при посещении объекта
*/

void Sprite::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    SpriteModel::AcceptCore (visitor);
}

/*
    Реализация получения количества спрайтов и массива спрайтов
*/

size_t Sprite::SpritesCountCore ()
{
  return 1;
}

const SpriteModel::SpriteDesc* Sprite::SpritesCore ()
{
  return &impl->sprite_desc;
}
