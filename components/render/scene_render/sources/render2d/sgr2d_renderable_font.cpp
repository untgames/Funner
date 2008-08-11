#include "shared.h"

using namespace render::render2d;

/*
   Подготовленные к рендерингу шрифт
*/

RenderableFont::RenderableFont (const char* font_name, Render& in_render)
  : font (font_name), render (in_render), max_glyph_side (0)
{
  if (font.ImageName () == '\0')
    throw xtl::format_operation_exception ("render::render2d::RenderableFont::RenderableFont", "Font '%s' doesn't contain texture name", font_name);
    
  RenderQueryPtr query;

  texture = render.GetTexture (font.ImageName (), true, query);

  sprites_buffer.resize (font.GlyphsTableSize ());

  media::GlyphInfo* current_glyph = font.Glyphs ();

  for (size_t i = 0; i < sprites_buffer.size (); i++, current_glyph++)
  {
    if (max_glyph_side < current_glyph->width)
      max_glyph_side = current_glyph->width;
    if (max_glyph_side < current_glyph->height)
      max_glyph_side = current_glyph->height;
  }

  current_glyph = font.Glyphs ();

  float texture_width = (float)texture->GetWidth (), texture_height = (float)texture->GetHeight ();

  render::mid_level::renderer2d::Sprite* current_sprite = sprites_buffer.data ();

  for (size_t i = 0; i < sprites_buffer.size (); i++, current_glyph++, current_sprite++)
  {
    current_sprite->size       = math::vec2f ((float)current_glyph->width / max_glyph_side, (float)current_glyph->height / max_glyph_side);
    current_sprite->tex_offset = math::vec2f ((float)current_glyph->x_pos / texture_width, (float)current_glyph->y_pos / texture_height);
    current_sprite->tex_size   = math::vec2f ((float)current_glyph->width / texture_width, (float)current_glyph->height / texture_height);
  }
}

RenderableFont::~RenderableFont ()
{
}

/*
   Получение спрайта по индексу
*/

const render::mid_level::renderer2d::Sprite& RenderableFont::GetSprite (size_t index) const
{
  if (index >= sprites_buffer.size ())
    throw xtl::make_range_exception ("renedr::render2d::RenderableFont::GetSprite", "index", index, 0u, sprites_buffer.size ());

  return sprites_buffer.data ()[index];
}
