#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;

/*
     онструктор / деструктор
*/

Primitive::Primitive ()
  : blend_mode (BlendMode_None),
    low_level_texture (0)
{
}

Primitive::~Primitive ()
{
  try
  {
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    ћатрица преобразований
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
    ”становка базовой текстуры
*/

void Primitive::SetTexture (render::mid_level::renderer2d::ITexture* in_texture)
{
  if (!in_texture)
  {
    texture = 0;
    return;
  }

  render::mid_level::low_level_driver::renderer2d::ImageTexture* image_texture = dynamic_cast<ImageTexture*> (in_texture);

  if (image_texture)
  {
    low_level_texture = image_texture->GetTexture ();      
    texture = image_texture;
  }
  else
  {
    render::mid_level::low_level_driver::renderer2d::RenderTargetTexture* render_target_texture = dynamic_cast<RenderTargetTexture*> (in_texture);

    render_target_texture = dynamic_cast<RenderTargetTexture*> (in_texture);

    if (!render_target_texture)
      throw xtl::make_argument_exception ("render::mid_level::low_level_driver::renderer2d::Primitive::SetTexture", "texture", typeid (in_texture).name (),
        "Texture type must be render::mid_level::low_level_driver::renderer2d::ImageTexture or render::mid_level::low_level_driver::renderer2d::RenderTargetTexture");

    low_level_texture = render_target_texture->GetView ()->GetTexture ();
    texture = render_target_texture;
  }
}

ITexture* Primitive::GetTexture ()
{
  return texture.get ();
}

/*
    –ежим смешивани€ цветов
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
      throw xtl::make_argument_exception ("render::mid_level::low_level_driver::renderer2d::Primitive::SetBlendMode", "blend_mode", in_blend_mode);
  }
  
  blend_mode = in_blend_mode;
}

BlendMode Primitive::GetBlendMode ()
{
  return blend_mode;
}

/*
   —прайты
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
    throw xtl::make_range_exception ("render::mid_level::low_level_driver::renderer2d::Primitive::GetSprite", "index", index, sprites.size ());

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

//резервирование места дл€ спрайтов
void Primitive::ReserveSprites (size_t sprites_count)
{
  sprites.reserve (sprites_count);
}
