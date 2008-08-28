#include "shared.h"

#undef min
#undef max

using namespace render::render2d;
using namespace scene_graph;

namespace
{

//границы текста
struct TextDimensions
{
  math::vec2f min;
  math::vec2f max;
};

//определение корректности кода символа
inline bool is_symbol_valid (size_t symbol_code, size_t first_code, size_t last_code)
{
  return symbol_code >= first_code && symbol_code < last_code;
}

}

/*
    Описание реализации строки текста
*/

struct RenderableTextLine::Impl
{
  Render&                        render;                       //ссылка на рендер
  scene_graph::TextLine*         text_line;                    //исходная линия текста
  PrimitivePtr                   primitive;                    //визуализируемый примитив
  SpritesBuffer                  sprites_buffer;               //буфер спрайтов
  math::vec4f                    current_color;                //текущий цвет
  RenderableFont*                current_renderable_font;      //текущий шрифт
  size_t                         current_text_hash;            //текущий хэш текста
  TextDimensions                 current_text_dimensions;      //текущие границы текста 
  scene_graph::TextLineAlignment current_horizontal_alignment; //текущее горизонтальное выравнивание
  scene_graph::TextLineAlignment current_vertical_alignment;   //текущее вертикальное выравнивание
  math::vec3f                    current_offset;               //текущее смещение текста, определяемое выраваниванием  
  size_t                         current_world_tm_hash;        //хэш текущей матрицы преобразований  
  bool                           need_full_update;             //необходимо полностью обновить все параметры
  bool                           wrong_state;                  //надпись находится в некорректном состоянии

///Конструктор
  Impl (scene_graph::TextLine* in_text_line, Render& in_render)
    : render (in_render),
      text_line (in_text_line),
      primitive (render.Renderer ()->CreatePrimitive (), false),
      need_full_update (true),
      wrong_state (true)
  {
    primitive->SetBlendMode (render::mid_level::renderer2d::BlendMode_Translucent);
  }
  
///Обновление параметров строки текста
  void Update ()
  {
    try
    {
      if (wrong_state)
        need_full_update = true;

      wrong_state = true;
      
        //получение шрифта
        
      RenderableFont* renderable_font = render.GetFont (text_line->Font ());
      
        //инициализация флагов обновления
        
      bool need_update_sprites = false;

        //определение необходимости обновления текста

      bool need_update_text = renderable_font != current_renderable_font || text_line->TextUnicodeHash () != current_text_hash;

        //определение необходимости обновления смещения текста

      bool need_update_offset = current_horizontal_alignment != text_line->HorizontalAlignment () ||
                                current_vertical_alignment != text_line->VerticalAlignment ();

        //проверка необходимости обновления матрицы преобразований текста

      const math::mat4f& world_tm      = text_line->WorldTM ();
      size_t             world_tm_hash = common::crc32 (&world_tm, sizeof (math::mat4f));

      bool need_update_tm = world_tm_hash != current_world_tm_hash;

        //определение необходимости обновления цвета текста

      bool need_update_color = current_color != text_line->Color ();
      
        //определение необходимости обновления всех полей
        
      if (need_full_update)
      {
        need_update_text    = true;
        need_update_offset  = true;
        need_update_color   = true;
        need_update_tm      = true;
        need_update_sprites = true;
      }

        //обновление текста

      if (need_update_text)
      {
          //инициализация параметров рендеринга текста

        const wchar_t*          text_unicode      = text_line->TextUnicode ();
        size_t                  text_length       = xtl::xstrlen (text_unicode);
        const media::Font&      font              = renderable_font->GetFont ();
        const media::GlyphInfo* glyphs            = font.Glyphs ();
        size_t                  glyphs_count      = font.GlyphsTableSize (),
                                first_glyph_code  = font.FirstGlyphCode (),
                                last_glyph_code   = first_glyph_code + glyphs_count;
        float                   current_pen_x     = 0.0f,
                                current_pen_y     = 0.0f,
                                max_glyph_side    = (float)renderable_font->GetMaxGlyphSide ();
        math::vec4f             color             = text_line->Color ();
        TextDimensions          text_dimensions;

          //резервирование места для буфера спрайтов

        sprites_buffer.resize (text_length, false);      

          //формирование массива спрайтов

        const wchar_t*                 pos              = text_unicode;
        size_t                         prev_glyph_index = 0;      
        mid_level::renderer2d::Sprite* dst_sprite       = sprites_buffer.data ();

        for (size_t i=0; i<text_length; i++, pos++)
        {
            //проверка наличия кода символа в шрифте

          size_t current_symbol_code = *pos;

          if (!is_symbol_valid (current_symbol_code, first_glyph_code, last_glyph_code))
          {
            if (!is_symbol_valid ('?', first_glyph_code, last_glyph_code))
              continue;

            current_symbol_code = '?';
          }
          
            //получение глифа

          size_t                  glyph_index = current_symbol_code - first_glyph_code;
          const media::GlyphInfo& glyph       = glyphs [glyph_index];
          
            //перенос пера
            
          if (dst_sprite != sprites_buffer.data ()) //производится только если есть предыдущий символ
          {
            if (font.HasKerning (prev_glyph_index, glyph_index))
            {
              media::KerningInfo kerning_info = font.Kerning (prev_glyph_index, glyph_index);

              current_pen_x += kerning_info.x_kerning / max_glyph_side;
              current_pen_y += kerning_info.y_kerning / max_glyph_side;
            }
          }

            //инициализация спрайта

              //bearing внести в инициализацию спрайтов в RenderableFont!!!!

          float bearing_x = glyph.bearing_x / max_glyph_side, bearing_y = glyph.bearing_y / max_glyph_side;

          const mid_level::renderer2d::Sprite& src_sprite = renderable_font->GetSprite (glyph_index);

          dst_sprite->position   = math::vec3f (current_pen_x + bearing_x + src_sprite.size.x / 2.f,
                                                current_pen_y + bearing_y - src_sprite.size.y / 2.f, 0.f);
          dst_sprite->color      = color;
          dst_sprite->size       = src_sprite.size;
          dst_sprite->tex_offset = src_sprite.tex_offset;
          dst_sprite->tex_size   = src_sprite.tex_size;        
          
            //перенос пера

          current_pen_x += glyph.advance_x / max_glyph_side;
          current_pen_y += glyph.advance_y / max_glyph_side;

            //корректировка границ текста

          TextDimensions glyph_dimensions;
          
          glyph_dimensions.min.x = dst_sprite->position.x - dst_sprite->size.x * 0.5f;
          glyph_dimensions.min.y = dst_sprite->position.y - dst_sprite->size.y * 0.5f;
          glyph_dimensions.max.x = glyph_dimensions.min.x + dst_sprite->size.x;
          glyph_dimensions.max.y = glyph_dimensions.min.y + dst_sprite->size.y;

          if (glyph_dimensions.min.x < text_dimensions.min.x) text_dimensions.min.x = glyph_dimensions.min.x;
          if (glyph_dimensions.min.y < text_dimensions.min.y) text_dimensions.min.y = glyph_dimensions.min.y;
          if (glyph_dimensions.max.x > text_dimensions.max.x) text_dimensions.max.x = glyph_dimensions.max.x;
          if (glyph_dimensions.max.y > text_dimensions.max.y) text_dimensions.max.y = glyph_dimensions.max.y;
          
            //переход к следующему спрайту

          dst_sprite++;
          prev_glyph_index = glyph_index;
        }
        
          //корректировка количества спрайтов

        sprites_buffer.resize (dst_sprite - sprites_buffer.data ());

          //обновление текстуры

        primitive->SetTexture (renderable_font->GetTexture ());      

          //обновление параметров

        current_text_dimensions = text_dimensions;      
        current_renderable_font = renderable_font;      
        current_text_hash       = text_line->TextUnicodeHash ();

          //обновление флагов

        need_update_offset  = true;
        need_update_sprites = true;
        need_update_color   = false;
      }
      
        //обновление положения текста
        
      if (need_update_offset)
      {
          //расчёт вектора смещения надписи
        
        scene_graph::TextLineAlignment halign = text_line->HorizontalAlignment (),
                                       valign = text_line->VerticalAlignment ();
        math::vec2f                    size   = current_text_dimensions.max - current_text_dimensions.min,
                                       offset = -current_text_dimensions.min;

        switch (halign)
        {
          default:
          case TextLineAlignment_Center:
            offset.x -= 0.5f * size.x;
            break;
          case TextLineAlignment_Right:
            offset.x -= size.x;
            break;
          case TextLineAlignment_Left:
            break;
        }
        
        switch (valign)
        {
          default:
          case TextLineAlignment_Center:
            offset.y -= 0.5f * size.y;
            break;
          case TextLineAlignment_Top:
            offset.y -= size.y;
            break;
          case TextLineAlignment_Bottom:
            break;
        }

          //обновление параметров

        current_horizontal_alignment = halign;
        current_vertical_alignment   = valign;
        current_offset               = math::vec3f (offset, 0.0f);

          //обновление флагов

        need_update_tm = true;
      }

        //обновление матрицы преобразований

      if (need_update_tm)
      {
        primitive->SetTransform (world_tm * math::translate (current_offset));

          //обновление параметров

        current_world_tm_hash = world_tm_hash;
      }

        //обновление цвета текста

      if (need_update_color)
      {
          //получение цвета
         
        math::vec4f color = text_line->Color ();
        
          //обновление цвета в спрайтах

        mid_level::renderer2d::Sprite* sprite = sprites_buffer.data ();

        for (size_t i=0, count=sprites_buffer.size (); i<count; i++, sprite++)
          sprite->color = color;

          //обновление параметров

        current_color = text_line->Color ();
        
          //обновление флагов
          
        need_update_sprites = true;
      }
      
        //обновление спрайтов
        
      if (need_update_sprites)
      {
        primitive->RemoveAllSprites ();
        primitive->AddSprites       (sprites_buffer.size (), sprites_buffer.data ());
      }

        //обновление корректности состояния надписи

      wrong_state = false;
    }
    catch (std::exception& exception)
    {
      render.LogPrintf ("%s\n    at render::render2d::RenderableTextLine::Update(text_line='%s')", exception.what (), text_line->Name ());
    }
    catch (...)
    {    
      render.LogPrintf ("Unknown exception\n    at render::render2d::RenderableTextLine::Update(text_line='%s')", text_line->Name ());
    }
  }
};

/*
    Конструктор
*/

RenderableTextLine::RenderableTextLine (scene_graph::TextLine* text_line, Render& render)
  : Renderable (text_line),
    impl (new Impl (text_line, render))
{
}

RenderableTextLine::~RenderableTextLine ()
{
}

/*
    Оповещения об обновлении
*/

void RenderableTextLine::Update ()
{
  impl->Update ();
}

/*
    Рисование
*/

void RenderableTextLine::DrawCore (IFrame& frame)
{
  if (!impl->wrong_state && impl->primitive->GetSpritesCount ())
    frame.AddPrimitive (impl->primitive.get ());
}
