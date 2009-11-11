#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;
using namespace render::mid_level::debug::renderer2d;

/*
    Конструктор / деструктор
*/

Primitive::Primitive ()
  : blend_mode (BlendMode_None)
  , alpha_reference (0.0f)
  , scissor_state (false)
{
  log.Printf ("Create primitive (id=%u)", Id ());
  
  scissor_rect.x      = 0;
  scissor_rect.y      = 0;
  scissor_rect.width  = 0;
  scissor_rect.height = 0;  
}

Primitive::~Primitive ()
{
  try
  {
    log.Printf ("Destroy primitive (id=%u)", Id ());
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Матрица преобразований
*/

void Primitive::SetTransform (const math::mat4f& in_transform)
{
  transform = in_transform;
}

void Primitive::GetTransform (math::mat4f& out_transform)
{
  out_transform = transform;
}

/*
    Установка базовой текстуры
*/

void Primitive::SetTexture (ITexture* in_texture)
{
  if (!in_texture)
  {
    texture = 0;
    return;
  }

  Texture* casted_texture = dynamic_cast<Texture*> (in_texture);

  if (!casted_texture)
    throw xtl::make_argument_exception ("render::mid_level::debug::renderer2d::Primitive::SetTexture", "texture", typeid (in_texture).name (),
      "Texture type is incompatible with render::mid_level::debug::renderer2d::Texture");
      
  texture = casted_texture;
}

ITexture* Primitive::GetTexture ()
{
  return texture.get ();
}

/*
    Режим смешивания цветов
*/

void Primitive::SetBlendMode (BlendMode in_blend_mode)
{
  switch (in_blend_mode)
  {
    case BlendMode_None:
    case BlendMode_Translucent:
    case BlendMode_Mask:
    case BlendMode_Additive:
      break;
    default:
      throw xtl::make_argument_exception ("render::mid_level::debug::renderer2d::Primitive::SetBlendMode", "blend_mode", in_blend_mode);
  }
  
  blend_mode = in_blend_mode;
}

BlendMode Primitive::GetBlendMode ()
{
  return blend_mode;
}

/*
    Установка параметра для работы альфа-теста
*/

void Primitive::SetAlphaReference (float ref)
{
  alpha_reference = ref;
}

float Primitive::GetAlphaReference ()
{
  return alpha_reference;
}

/*
    Область отсечения
*/

void Primitive::SetScissor (const Viewport& viewport)
{
  scissor_rect.x      = viewport.x;
  scissor_rect.y      = viewport.y;
  scissor_rect.width  = viewport.width;
  scissor_rect.height = viewport.height;
}

void Primitive::SetScissorState (bool state)
{
  if (scissor_state == state)
    return;

  scissor_state = state;    
}

void Primitive::GetScissor (Viewport& out_viewport)
{
  out_viewport.x      = scissor_rect.x;
  out_viewport.y      = scissor_rect.y;
  out_viewport.width  = scissor_rect.width;
  out_viewport.height = scissor_rect.height; 
}

bool Primitive::GetScissorState ()
{
  return scissor_state;
}

/*
    Спрайты
*/

//количество спрайтов
size_t Primitive::GetSpritesCount ()
{
  return sprites.size ();
}

//получение спрайта
void Primitive::GetSprite (size_t index, Sprite& sprite)
{
  if (index >= sprites.size ())
    throw xtl::make_range_exception ("render::mid_level::debug::renderer2d::Primitive::GetSprite", "index", index, sprites.size ());

  sprite = sprites [index];
}

//добавление спрайтов
size_t Primitive::AddSprites (size_t sprites_count, const Sprite* sprites_array)
{
  size_t first = sprites.size ();

  sprites.insert (sprites.begin (), sprites_array, sprites_array + sprites_count);
  
  return first;
}

//удаление спрайтов
void Primitive::RemoveSprites (size_t first_sprite, size_t sprites_count)
{
  if (first_sprite >= sprites.size ())
    return;
    
  if (first_sprite > sprites.size () - sprites_count)
    sprites_count = sprites.size () - first_sprite;

  sprites.erase (sprites.begin () + first_sprite, sprites.begin () + first_sprite + sprites_count);  
}

//удаление всех спрайтов
void Primitive::RemoveAllSprites ()
{
  sprites.clear ();
}

//резервирование места для спрайтов
void Primitive::ReserveSprites (size_t sprites_count)
{
  sprites.reserve (sprites_count);
}
