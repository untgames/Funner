#include "shared.h"

using namespace render::obsolete::render2d;

struct RenderableFont::Impl
{
  Render&       render;         //ссылка на рендер
  media::Font   font;           //шрифт
  SpritesBuffer sprites_buffer; //буффер спрайтов (по спрайту для каждого глифа)
  TexturePtr    texture;        //текстура

  Impl (const char* font_name, Render& in_render)
    : render (in_render),
      font (font_name)
  {
    size_t glyphs_count = font.GlyphsTableSize ();

      //загрузка текстуры, содержащей битовые карты символов    

    texture = render.GetTexture (font.ImageName (), true);
    
    texture->SetMinFilter (render::mid_level::renderer2d::TexMinFilter_LinearMipLinear);

      //получение размеров текстуры

    size_t texture_width  = texture->GetWidth (),
           texture_height = texture->GetHeight ();
          
      //формирование массива спрайтов

    sprites_buffer.resize (font.GlyphsTableSize ());

    render::mid_level::renderer2d::Sprite* current_sprite = sprites_buffer.data ();
    
    media::GlyphInfo *current_glyph = font.Glyphs ();

    size_t font_size = font.FontSize ();

    for (size_t i=0; i<glyphs_count; i++, current_glyph++, current_sprite++)
    {
      current_sprite->size       = math::vec2f ((float)current_glyph->width / font_size, (float)current_glyph->height / font_size);
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
   Получение спрайта по индексу
*/

const render::mid_level::renderer2d::Sprite& RenderableFont::GetSprite (size_t index) const
{
  if (index >= impl->sprites_buffer.size ())
    throw xtl::make_range_exception ("render::render2d::RenderableFont::GetSprite", "index", index, impl->sprites_buffer.size ());

  return impl->sprites_buffer.data ()[index];
}
