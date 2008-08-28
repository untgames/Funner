#include "shared.h"

using namespace render::render2d;

struct RenderableFont::Impl
{
  Render&       render;         //ссылка на рендер
  media::Font   font;           //шрифт
  SpritesBuffer sprites_buffer; //буффер спрайтов (по спрайту для каждого глифа)
  TexturePtr    texture;        //текстура
  size_t        max_glyph_side; //размер самой большой стороны среди всех глифов (в пикселях)

  Impl (const char* font_name, Render& in_render)
    : font (font_name),
      render (in_render),
      max_glyph_side (0)
  {
    size_t glyphs_count = font.GlyphsTableSize ();

      //загрузка текстуры, содержащей битовые карты символов
    
    RenderQueryPtr query;

    texture = render.GetTexture (font.ImageName (), true, query);         
    
      //определение максимальной стороны глифов

    const media::GlyphInfo* current_glyph = font.Glyphs ();

    for (size_t i=0; i<glyphs_count; i++, current_glyph++)
    {
      size_t current_glyph_width  = stl::max ((int)current_glyph->width,  abs (current_glyph->advance_x)),
             current_glyph_height = stl::max ((int)current_glyph->height, abs (current_glyph->advance_y));

      if (max_glyph_side < current_glyph_width)  max_glyph_side = current_glyph_width;
      if (max_glyph_side < current_glyph_height) max_glyph_side = current_glyph_height;
    }

      //получение размеров текстуры

    size_t texture_width = texture->GetWidth (),
          texture_height = texture->GetHeight ();
          
      //формирование массива спрайтов

    sprites_buffer.resize (font.GlyphsTableSize ());

    render::mid_level::renderer2d::Sprite* current_sprite = sprites_buffer.data ();
    
    current_glyph = font.Glyphs ();

    for (size_t i=0; i<glyphs_count; i++, current_glyph++, current_sprite++)
    {
      current_sprite->size       = math::vec2f ((float)current_glyph->width / max_glyph_side, (float)current_glyph->height / max_glyph_side);
      current_sprite->tex_offset = math::vec2f ((float)current_glyph->x_pos / texture_width, (float)current_glyph->y_pos / texture_height);
      current_sprite->tex_size   = math::vec2f ((float)current_glyph->width / texture_width, (float)current_glyph->height / texture_height);
    }
  }
};


/*
   Подготовленные к рендерингу шрифт
*/

RenderableFont::RenderableFont (const char* font_name, Render& render)
  : impl (new Impl (font_name, render))
{
}

RenderableFont::~RenderableFont ()
{
}

/*
   Получение шрифта
*/

const media::Font& RenderableFont::GetFont () const 
{ 
  return impl->font; 
}

/*
   Получение текстуры
*/

ITexture* RenderableFont::GetTexture () const 
{ 
  return impl->texture.get (); 
}

/*
   Получение размера самой большой стороны глифа
*/

size_t RenderableFont::GetMaxGlyphSide () const 
{ 
  return impl->max_glyph_side; 
}

/*
   Получение спрайта по индексу
*/

const render::mid_level::renderer2d::Sprite& RenderableFont::GetSprite (size_t index) const
{
  if (index >= impl->sprites_buffer.size ())
    throw xtl::make_range_exception ("render::render2d::RenderableFont::GetSprite", "index", index, impl->sprites_buffer.size ());

  return impl->sprites_buffer.data ()[index];
}
