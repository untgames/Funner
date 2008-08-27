#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::window_driver;
using namespace render::mid_level::window_driver::renderer2d;

/*
    Конструктор / деструктор
*/

Primitive::Primitive ()
  : need_update_transform (true),
    need_update_renderable_sprites (true)
{
  renderable_primitive.blend_mode      = BlendMode_None;
  renderable_primitive.texture         = 0;
  renderable_primitive.alpha_reference = 0.0f;
}

/*
    Матрица преобразований
*/

void Primitive::SetTransform (const math::mat4f& in_transform)
{
  transform = in_transform;

  need_update_transform = true;
}

void Primitive::GetTransform (math::mat4f& out_transform)
{
  out_transform = transform;
}

/*
    Установка базовой текстуры
*/

void Primitive::SetTexture (render::mid_level::renderer2d::ITexture* in_texture)
{
  if (!in_texture)
  {
    renderable_primitive.texture = 0;
    return;
  }

  if (ImageTexture* image_texture = dynamic_cast<ImageTexture*> (in_texture))
  {
    renderable_primitive.texture = image_texture->GetTexture ();      
    texture                      = image_texture;
  }
  else if (RenderTargetTexture* render_target_texture = dynamic_cast<RenderTargetTexture*> (in_texture))
  {
    renderable_primitive.texture = render_target_texture->GetView ()->GetTexture ();
    texture                      = render_target_texture;
  }
  else
  {
    throw xtl::make_argument_exception ("render::mid_level::window_driver::renderer2d::Primitive::SetTexture", "texture", typeid (in_texture).name (),
      "Texture type must be render::mid_level::window_driver::renderer2d::ImageTexture or render::mid_level::window_driver::renderer2d::RenderTargetTexture");    
  }
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
    case BlendMode_AlphaClamp:
      break;
    default:
      throw xtl::make_argument_exception ("render::mid_level::window_driver::renderer2d::Primitive::SetBlendMode", "blend_mode", in_blend_mode);
  }

  renderable_primitive.blend_mode = in_blend_mode;
}

BlendMode Primitive::GetBlendMode ()
{
  return renderable_primitive.blend_mode;
}

/*
    Установка параметра для работы альфа-теста
*/

void Primitive::SetAlphaReference (float ref)
{
  renderable_primitive.alpha_reference = ref;
}

float Primitive::GetAlphaReference ()
{
  return renderable_primitive.alpha_reference;
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
    throw xtl::make_range_exception ("render::mid_level::window_driver::renderer2d::Primitive::GetSprite", "index", index, sprites.size ());

  sprite = sprites [index];
}

//добавление спрайтов
size_t Primitive::AddSprites (size_t sprites_count, const Sprite* sprites_array)
{
  size_t first = sprites.size ();

  if (!sprites_count)
    return first;

  if (!sprites_array)
    throw xtl::make_null_argument_exception ("render::mid_level::window_driver::Primitive::AddSprites", "sprites_array");

  sprites.insert (sprites.end (), sprites_array, sprites_array + sprites_count);
  
  need_update_renderable_sprites = true;

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

  need_update_renderable_sprites = true;
}

//удаление всех спрайтов
void Primitive::RemoveAllSprites ()
{
  sprites.clear ();
  
  need_update_renderable_sprites = true;
}

//резервирование места для спрайтов
void Primitive::ReserveSprites (size_t sprites_count)
{
  sprites.reserve (sprites_count);
}

/*
    Обновление визуализируемых спрайтов
*/

void Primitive::UpdateRenderableSprites ()
{
    //обновление содержимого массива спрайтов

  if (need_update_renderable_sprites)
  {
      //изменение размера массива спрайтов

    renderable_sprites.resize (sprites.size (), false);
    
      //добавление спрайтов      
      
    const Sprite*     src_sprite = &sprites [0];
    RenderableSprite* dst_sprite = renderable_sprites.data ();

    for (size_t count=sprites.size (); count--; src_sprite++, dst_sprite++)
    {
      const math::vec2f &offset = src_sprite->tex_offset,
                        &size   = src_sprite->tex_size;
      RenderableVertex* verts   = dst_sprite->vertices;

      dst_sprite->primitive = &renderable_primitive;

      verts [0].texcoord = offset;
      verts [1].texcoord = math::vec2f (offset.x + size.x, offset.y);
      verts [2].texcoord = math::vec2f (offset.x, offset.y + size.y);
      verts [3].texcoord = offset + size;
      verts [4].texcoord = verts [2].texcoord;
      verts [5].texcoord = verts [1].texcoord;

      const math::vec4f &color = src_sprite->color;      

      for (size_t i=0; i<SPRITE_VERTICES_COUNT; i++)
        verts [i].color = color;
    }    
    
    need_update_renderable_sprites = false;
    need_update_transform          = true;    
  }

    //обновление трансформаций
  
  if (need_update_transform)
  {
    const Sprite*     src_sprite = &sprites [0];
    RenderableSprite* dst_sprite = renderable_sprites.data ();

    for (size_t count=sprites.size (); count--; src_sprite++, dst_sprite++)
    {
      const math::vec3f& pos   = src_sprite->position;
      math::vec2f        size  = src_sprite->size * 0.5f;
      RenderableVertex*  verts = dst_sprite->vertices;

      verts [0].position = transform * math::vec3f (pos.x - size.x, pos.y - size.y, pos.z);
      verts [1].position = transform * math::vec3f (pos.x + size.x, pos.y - size.y, pos.z);
      verts [2].position = transform * math::vec3f (pos.x - size.x, pos.y + size.y, pos.z);
      verts [3].position = transform * math::vec3f (pos.x + size.x, pos.y + size.y, pos.z);
      verts [4].position = verts [2].position;
      verts [5].position = verts [1].position;
    }    

    need_update_transform = false;
  }  
}

/*
    Добавление спрайтов в буфер
*/

void Primitive::AddSprites (RenderableSpriteList& list)
{
    //обновление визуализируемых спрайтов

  if (need_update_renderable_sprites || need_update_transform)
    UpdateRenderableSprites ();

    //добавление спрайтов в список визуализации

  list.Add (renderable_sprites.size (), renderable_sprites.data ());
}
