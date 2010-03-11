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
  , shader_mode (ShaderMode_Normal)
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
    Режим шейдинга
*/

void Primitive::SetShaderMode (ShaderMode in_shader_mode)
{
  switch (in_shader_mode)
  {
    case ShaderMode_Normal:
    case ShaderMode_AlphaClamp:
    case ShaderMode_Reflection:
      break;
    default:
      throw xtl::make_argument_exception ("render::mid_level::debug::renderer2d::Primitive::SetShaderMode", "shader_mode", in_shader_mode);
  }
  
  shader_mode = in_shader_mode;
}

ShaderMode Primitive::GetShaderMode ()
{
  return shader_mode;
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
  return vertices.size () / 4;
}

//добавление спрайтов
void Primitive::AddSprites (size_t sprites_count, const Sprite* sprites_array)
{
  if (!sprites_count)
    return;

  if (!sprites_array)
    throw xtl::make_null_argument_exception ("render::mid_level::debug::render2d::Primitive::AddSprites", "sprites_array");

  stl::vector<SpriteVertex> verts (sprites_count * 4);
  
  for (size_t i=0; i<sprites_count; i++)
  {
    const Sprite& sprite = sprites_array [i];
    SpriteVertex* vert   = &verts [i * 4];
    
    vert [0].position = math::vec3f (sprite.position.x - sprite.size.x / 2.0f, sprite.position.y + sprite.size.y / 2.0f, sprite.position.z);
    vert [1].position = math::vec3f (sprite.position.x + sprite.size.x / 2.0f, sprite.position.y + sprite.size.y / 2.0f, sprite.position.z);
    vert [2].position = math::vec3f (sprite.position.x + sprite.size.x / 2.0f, sprite.position.y - sprite.size.y / 2.0f, sprite.position.z);
    vert [3].position = math::vec3f (sprite.position.x - sprite.size.x / 2.0f, sprite.position.y - sprite.size.y / 2.0f, sprite.position.z);

    vert [0].tex_coord = math::vec2f (sprite.tex_offset.x, sprite.tex_offset.y + sprite.tex_size.y);
    vert [1].tex_coord = sprite.tex_offset + sprite.tex_size;
    vert [2].tex_coord = math::vec2f (sprite.tex_offset.x + sprite.tex_size.x, sprite.tex_offset.y);
    vert [3].tex_coord = sprite.tex_offset;
    
    for (size_t j=0; j<4; j++)
    {
      vert [j].color  = sprite.color;
      vert [j].normal = math::vec3f (0, 0, 1.0f);
    }
  }

  AddSprites (sprites_count, &verts [0]);
}

void Primitive::AddSprites (size_t sprites_count, const SpriteVertex* verts)
{
  if (!sprites_count)
    return;

  if (!verts)
    throw xtl::make_null_argument_exception ("render::mid_level::debug::render2d::Primitive::AddSprites", "vers");

  vertices.insert (vertices.begin (), verts, verts + sprites_count * 4);
}

//удаление всех спрайтов
void Primitive::RemoveAllSprites ()
{
  vertices.clear ();
}

//резервирование места для спрайтов
void Primitive::ReserveSprites (size_t sprites_count)
{
  vertices.reserve (sprites_count * 4);
}

/*
    Получение вершин спрайта
*/

const SpriteVertex* Primitive::Vertices () const
{
  return vertices.empty () ? 0 : &vertices [0];
}
