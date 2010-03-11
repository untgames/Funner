#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::window_driver;
using namespace render::mid_level::window_driver::renderer2d;

/*
    Конструктор / деструктор
*/

Primitive::Primitive ()
  : need_update_transform (true)
  , need_update_renderable_sprites (true)
  , scissor_state (false)
{
  scissor_rect.x      = 0;
  scissor_rect.y      = 0;
  scissor_rect.width  = 0;
  scissor_rect.height = 0;

  renderable_primitive.blend_mode      = BlendMode_None;
  renderable_primitive.texture         = 0;
  renderable_primitive.alpha_reference = 0.0f;
  renderable_primitive.scissor         = 0;
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

  scissor_state                = state;    
  renderable_primitive.scissor = state ? &scissor_rect : 0;
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
  return sprite_vertices.size () / SPRITE_VERTICES_COUNT;
}

//добавление спрайтов
void Primitive::AddSprites (size_t sprites_count, const Sprite* sprites_array)
{
  if (!sprites_count)
    return;

  if (!sprites_array)
    throw xtl::make_null_argument_exception ("render::mid_level::window_driver::Primitive::AddSprites", "sprites_array");
    
  size_t first = sprite_vertices.size ();
    
  sprite_vertices.resize (sprite_vertices.size () + sprites_count * SPRITE_VERTICES_COUNT);

  const Sprite* sprite = sprites_array;
  SpriteVertex* vertex = &sprite_vertices [first];

  for (size_t i=0; i<sprites_count; i++, sprite++, vertex += SPRITE_VERTICES_COUNT)
  {
    const math::vec2f &offset = sprite->tex_offset,
                      &size   = sprite->tex_size;
                      
    vertex [0].position = math::vec3f (sprite->position.x - sprite->size.x / 2.0f, sprite->position.y + sprite->size.y / 2.0f, sprite->position.z);
    vertex [1].position = math::vec3f (sprite->position.x + sprite->size.x / 2.0f, sprite->position.y + sprite->size.y / 2.0f, sprite->position.z);
    vertex [2].position = math::vec3f (sprite->position.x + sprite->size.x / 2.0f, sprite->position.y - sprite->size.y / 2.0f, sprite->position.z);
    vertex [3].position = math::vec3f (sprite->position.x - sprite->size.x / 2.0f, sprite->position.y - sprite->size.y / 2.0f, sprite->position.z);

    vertex [0].tex_coord = math::vec2f (offset.x, offset.y + size.y);
    vertex [1].tex_coord = offset + size;
    vertex [2].tex_coord = math::vec2f (offset.x + size.x, offset.y);
    vertex [3].tex_coord = offset;

    for (size_t j=0; j<SPRITE_VERTICES_COUNT; j++)
    {
      vertex [j].color  = sprite->color;
      vertex [j].normal = math::vec3f (0, 0, 1.0f);
    }
  }
  
  need_update_renderable_sprites = true;
}

void Primitive::AddSprites (size_t sprites_count, const SpriteVertex* verts)
{
  if (!sprites_count)
    return;

  if (!verts)
    throw xtl::make_null_argument_exception ("render::mid_level::window_driver::Primitive::AddSprites", "verts");

  sprite_vertices.insert (sprite_vertices.begin (), verts, verts + sprites_count * SPRITE_VERTICES_COUNT);
  
  need_update_renderable_sprites = true;
}

//удаление всех спрайтов
void Primitive::RemoveAllSprites ()
{
  sprite_vertices.clear ();
  
  need_update_renderable_sprites = true;
}

//резервирование места для спрайтов
void Primitive::ReserveSprites (size_t sprites_count)
{
  sprite_vertices.reserve (sprites_count * SPRITE_VERTICES_COUNT);
}

/*
    Обновление визуализируемых спрайтов
*/

void Primitive::UpdateRenderableSprites ()
{
    //обновление содержимого массива спрайтов
    
  size_t sprites_count = sprite_vertices.size () / SPRITE_VERTICES_COUNT;

  if (need_update_renderable_sprites)
  {
      //изменение размера массива спрайтов
      
    renderable_sprites.resize (sprites_count, false);
    
      //добавление спрайтов
      
    const SpriteVertex* src_vertex = &sprite_vertices [0];
    RenderableSprite*   dst_sprite = renderable_sprites.data ();

    for (size_t i=0; i<sprites_count; i++, dst_sprite++)
    {
      RenderableVertex* dst_vertex = dst_sprite->vertices;

      dst_sprite->primitive = &renderable_primitive;
      
      for (size_t j=0; j<SPRITE_VERTICES_COUNT; j++, dst_vertex++, src_vertex++)
      {
        dst_vertex->position  = src_vertex->position;
        dst_vertex->normal    = src_vertex->normal;
        dst_vertex->texcoord  = src_vertex->tex_coord;
        dst_vertex->color     = math::vec4ub ((unsigned char)(src_vertex->color.x * 255.0f), (unsigned char)(src_vertex->color.y * 255.0f),
         (unsigned char)(src_vertex->color.z * 255.0f), (unsigned char)(src_vertex->color.w * 255.0f));
      }
    }
    
    need_update_renderable_sprites = false;
    need_update_transform          = true;    
  }

    //обновление трансформаций
  
  if (need_update_transform)
  {
    const SpriteVertex* src_vertex = &sprite_vertices [0];
    RenderableSprite*   dst_sprite = renderable_sprites.data ();

    for (size_t i=0; i<sprites_count; i++, dst_sprite++)
    {
      RenderableVertex* dst_vertex = dst_sprite->vertices;
      
      for (size_t j=0; j<SPRITE_VERTICES_COUNT; j++, dst_vertex++, src_vertex++)
      {
        dst_vertex->position = transform * src_vertex->position;
        dst_vertex->normal   = transform * math::vec4f (src_vertex->normal, 0.0f);
      }
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
