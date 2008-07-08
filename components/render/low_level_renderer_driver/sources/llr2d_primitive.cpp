#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;

namespace
{

const size_t DEFAULT_SPRITE_VERTEX_BUFFER_SIZE = 4; //размер вершинного буффера по умолчанию

}

/*
    Конструктор / деструктор
*/

Primitive::Primitive ()
  : blend_mode (BlendMode_None),
    low_level_texture (0),
    sprite_vertex_buffer (DEFAULT_SPRITE_VERTEX_BUFFER_SIZE * sizeof (SpriteVertexData)),
    dirty_transform (false)
{
}

/*
    Матрица преобразований
*/

void Primitive::SetTransform (const math::mat4f& in_transform)
{
  transform = in_transform;

  dirty_transform = true;
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

  for (SpriteVertexData* iter = sprite_vertex_buffer.data (), *end = sprite_vertex_buffer.data () + sprite_vertex_buffer.size (); iter < end; iter++)
    iter->texture = low_level_texture;
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
      throw xtl::make_argument_exception ("render::mid_level::low_level_driver::renderer2d::Primitive::SetBlendMode", "blend_mode", in_blend_mode);
  }
  
  blend_mode = in_blend_mode;

  for (SpriteVertexData* iter = sprite_vertex_buffer.data (), *end = sprite_vertex_buffer.data () + sprite_vertex_buffer.size (); iter < end; iter++)
    iter->blend_mode = blend_mode;
}

BlendMode Primitive::GetBlendMode ()
{
  return blend_mode;
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
    throw xtl::make_range_exception ("render::mid_level::low_level_driver::renderer2d::Primitive::GetSprite", "index", index, sprites.size ());

  sprite = sprites [index];
}

//добавление спрайтов
size_t Primitive::AddSprites (size_t sprites_count, const Sprite* sprites_array)
{
  size_t first = sprites.size ();

  sprites.insert (sprites.end (), sprites_array, sprites_array + sprites_count);

  try
  {
    if (sprite_vertex_buffer.capacity () < (sprites.size ()  * sizeof (SpriteVertexData)))
      sprite_vertex_buffer.reserve ((sprites.size () + DEFAULT_SPRITE_VERTEX_BUFFER_SIZE) * sizeof (SpriteVertexData));

    sprite_vertex_buffer.resize (sprites.size () * sizeof (SpriteVertexData));
  }
  catch (xtl::exception& e)
  {
    sprites.resize (first);
    e.touch ("render::mid_level::low_level_driver::Primitive::AddSprites");
    throw;
  }

  for (size_t i = first; i < sprites.size (); i++)
    BuildSpriteVertexData (i);
  
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

  sprite_vertex_buffer.resize (sprites.size () * sizeof (SpriteVertexData));

  for (size_t i = 0; i < sprites.size (); i++)
    BuildSpriteVertexData (i);
}

//удаление всех спрайтов
void Primitive::RemoveAllSprites ()
{
  sprites.clear ();
  sprite_vertex_buffer.resize (0);
}

//резервирование места для спрайтов
void Primitive::ReserveSprites (size_t sprites_count)
{
  sprites.reserve (sprites_count);

  if (sprite_vertex_buffer.capacity () < (sprites_count * sizeof (SpriteVertexData)))
    sprite_vertex_buffer.reserve (sprites_count * sizeof (SpriteVertexData));
}

/*
   Получение данных для отрисовки
*/

SpriteVertexData* Primitive::GetSpriteVertexBuffer () 
{ 
  if (dirty_transform)
  {
    for (size_t i = 0; i < sprites.size (); i++)
      ComputeSpriteTransorm (i);

    dirty_transform = false;
  }

  return sprite_vertex_buffer.data (); 
}

/*
   Построение вершинных данных для спрайта
*/

void Primitive::BuildSpriteVertexData (size_t i)
{
  ComputeSpriteTransorm (i);

  sprite_vertex_buffer.data ()[i].color      = sprites[i].color;
  sprite_vertex_buffer.data ()[i].texture    = low_level_texture;
  sprite_vertex_buffer.data ()[i].blend_mode = blend_mode;

  sprite_vertex_buffer.data ()[i].vertices[0].texcoord = math::vec2f (sprites[i].tex_offset.x,                         sprites[i].tex_offset.y);
  sprite_vertex_buffer.data ()[i].vertices[1].texcoord = math::vec2f (sprites[i].tex_offset.x + sprites[i].tex_size.x, sprites[i].tex_offset.y);
  sprite_vertex_buffer.data ()[i].vertices[2].texcoord = math::vec2f (sprites[i].tex_offset.x,                         sprites[i].tex_offset.y + sprites[i].tex_size.y);
  sprite_vertex_buffer.data ()[i].vertices[3].texcoord = math::vec2f (sprites[i].tex_offset.x + sprites[i].tex_size.x, sprites[i].tex_offset.y + sprites[i].tex_size.y);
}

void Primitive::ComputeSpriteTransorm (size_t i)
{
  sprite_vertex_buffer.data ()[i].vertices[0].position = transform * math::vec3f (sprites[i].position.x - sprites[i].size.x, sprites[i].position.y - sprites[i].size.y, sprites[i].position.z);
  sprite_vertex_buffer.data ()[i].vertices[1].position = transform * math::vec3f (sprites[i].position.x + sprites[i].size.x, sprites[i].position.y - sprites[i].size.y, sprites[i].position.z);
  sprite_vertex_buffer.data ()[i].vertices[2].position = transform * math::vec3f (sprites[i].position.x - sprites[i].size.x, sprites[i].position.y + sprites[i].size.y, sprites[i].position.z);
  sprite_vertex_buffer.data ()[i].vertices[3].position = transform * math::vec3f (sprites[i].position.x + sprites[i].size.x, sprites[i].position.y + sprites[i].size.y, sprites[i].position.z);
}
