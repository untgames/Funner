#include "shared.h"

using namespace render::render2d;
using namespace scene_graph;

namespace
{

bool is_symbol_valid (size_t symbol_code, const media::Font& font)
{
  return ((symbol_code >= font.FirstGlyphCode ()) && (symbol_code < (font.FirstGlyphCode () + font.GlyphsTableSize ())));
}

struct TextDimensions
{
  float right;
  float left;
  float top;
  float bottom;
};

TextDimensions calculate_text_dimensions (render::mid_level::renderer2d::Sprite* glyphs, size_t glyphs_count)
{
  TextDimensions return_value = {0.f, 0.f, 0.f, 0.f};

  for (size_t i = 0; i < glyphs_count; i++, glyphs++)
  {
    if ((glyphs->position.x - (glyphs->size.x / 2.f)) < return_value.left)   return_value.left   = glyphs->position.x - (glyphs->size.x / 2.f);
    if ((glyphs->position.x + (glyphs->size.x / 2.f)) > return_value.right)  return_value.right  = glyphs->position.x + (glyphs->size.x / 2.f);
    if ((glyphs->position.y - (glyphs->size.y / 2.f)) < return_value.bottom) return_value.bottom = glyphs->position.y - (glyphs->size.y / 2.f);
    if ((glyphs->position.y + (glyphs->size.y / 2.f)) > return_value.top)    return_value.top    = glyphs->position.y + (glyphs->size.y / 2.f);
  }

  return return_value;
}

}

struct RenderableTextLine::Impl
{
  Render&                        render;                       //ссылка на рендер
  scene_graph::TextLine*         text_line;                    //исходная линия текста
  PrimitivePtr                   primitive;                    //визуализируемый примитив
  math::vec4f                    current_color;                //текущий цвет
  scene_graph::TextLineAlignment current_horizontal_alignment; //текущее горизонтальное выравнивание
  scene_graph::TextLineAlignment current_vertical_alignment;   //текущее вертикальное выравнивание
  math::mat4f                    translate_tm;                 //матрица сдвига для выравнивания
  math::mat4f                    current_world_tm;             //текущая матрица трансформации
  size_t                         current_world_tm_hash;        //хэш текущей матрицы трансформации
  stl::string                    current_font_name;            //текущий шрифт
  stl::string                    current_text;                 //текущий текст
  SpritesBuffer                  sprites_buffer;               //буффер спрайтов
  stl::vector<size_t>            glyph_indices;                //индексы глифов в буффере спрайтов
  TextDimensions                 text_dimensions;              //границы текста

  Impl (scene_graph::TextLine* in_text_line, Render& in_render)
    : render (in_render),
      text_line (in_text_line),
      primitive (render.Renderer ()->CreatePrimitive (), false),
      current_color (math::vec4f (-1.f)),
      current_horizontal_alignment (TextLineAlignment_Num),
      current_vertical_alignment (TextLineAlignment_Num),
      translate_tm (1)
  {
    primitive->SetBlendMode (render::mid_level::renderer2d::BlendMode_Translucent);
  }

  void Update ()
  {
    try
    {    
      bool need_rebuild_text    = false;
      bool need_update_position = false;
      bool need_color_update    = current_color != text_line->Color ();

      RenderableFont*    font         = render.GetFont (text_line->Font ());
      const media::Font& current_font = font->GetFont ();

        //проверка обновления шрифта
      if (xtl::xstrcmp (current_font_name.c_str (), text_line->Font ()))
      {
        //получение нового шрифта
        primitive->SetTexture (font->GetTexture ());
        
        need_rebuild_text    = true;
        need_update_position = true;
        need_color_update    = true;

        current_font_name = text_line->Font ();
      }

        //проверка обновления текста
      if (xtl::xstrcmp (current_text.c_str (), text_line->Text ()))
      {
        current_text = text_line->Text ();

        need_rebuild_text    = true;
        need_update_position = true;
        need_color_update    = true;
      }

        //формирование буфера спрайтов
      if (need_rebuild_text)
      {
        size_t text_length = strlen (text_line->Text ());

        sprites_buffer.resize (text_length, false);
        glyph_indices.resize (text_length);

        render::mid_level::renderer2d::Sprite* current_sprite = sprites_buffer.data ();

        size_t result_text_length = 0;

        for (size_t i = 0; i < text_length; i++, current_sprite++, result_text_length++)
        {
          size_t current_symbol_code = *(unsigned char*)&text_line->Text ()[i];

          if (!is_symbol_valid (current_symbol_code, current_font))
          {
            if (!is_symbol_valid ('?', current_font))
            {
              result_text_length--;
              
              continue;
            }
            else
              current_symbol_code = '?';
          }

          size_t glyph_index = current_symbol_code - current_font.FirstGlyphCode ();

          *current_sprite                   = font->GetSprite (glyph_index);
          glyph_indices[result_text_length] = glyph_index;
        }

        sprites_buffer.resize (result_text_length);

          //формирование позиций спрайтов
        current_sprite = sprites_buffer.data ();

        float current_pen_x_position = 0.f;
        float current_pen_y_position = 0.f;
        float max_glyph_side         = (float)font->GetMaxGlyphSide ();

        const media::GlyphInfo* glyphs = current_font.Glyphs ();

        for (size_t i = 0; i < sprites_buffer.size (); i++, current_sprite++)
        {
          float bearing_x = (float)glyphs [glyph_indices [i]].bearing_x / max_glyph_side;
          float bearing_y = (float)glyphs [glyph_indices [i]].bearing_y / max_glyph_side;

          current_sprite->position = math::vec3f (current_pen_x_position + bearing_x + current_sprite->size.x / 2.f, current_pen_y_position + bearing_y - current_sprite->size.y / 2.f, 0.f);

          if (((i + 1) < sprites_buffer.size ()) && (current_font.HasKerning (glyph_indices [i], glyph_indices [i + 1])))
          {
            media::KerningInfo kerning_info = current_font.Kerning (glyph_indices [i], glyph_indices [i + 1]);

            current_pen_x_position += (float)glyphs [glyph_indices [i]].advance_x / max_glyph_side + (float)kerning_info.x_kerning / max_glyph_side;
            current_pen_y_position += (float)glyphs [glyph_indices [i]].advance_y / max_glyph_side + (float)kerning_info.y_kerning / max_glyph_side;
          }
          else
          {
            current_pen_x_position += (float)glyphs [glyph_indices [i]].advance_x / max_glyph_side;
            current_pen_y_position += (float)glyphs [glyph_indices [i]].advance_y / max_glyph_side;
          }
        }

        text_dimensions = calculate_text_dimensions (sprites_buffer.data (), sprites_buffer.size ());
      }

      if ((current_horizontal_alignment != text_line->HorizontalAlignment ()) || (current_vertical_alignment != text_line->VerticalAlignment ()))
      {
        current_horizontal_alignment = text_line->HorizontalAlignment ();
        current_vertical_alignment   = text_line->VerticalAlignment ();

        need_update_position = true;
      }

      if (need_update_position)
      {
        math::vec3f translate_vector (0);

        if ((text_dimensions.left < -0.501f) || (text_dimensions.right > 0.501f))  //проверка вертикального шрифта
        {
          switch (current_horizontal_alignment)
          {
            case TextLineAlignment_Center:
              translate_vector.x = -(text_dimensions.right + text_dimensions.left) / 2.f;
              break;
            case TextLineAlignment_Right:
              translate_vector.x = -text_dimensions.right;
              break;
            case TextLineAlignment_Left:
              translate_vector.x = -text_dimensions.left;
              break;
          }
        }
        else
        {
          switch (current_horizontal_alignment)
          {
            case TextLineAlignment_Right:
              translate_vector.x = -0.5;
              break;
            case TextLineAlignment_Left:
              translate_vector.x = 0.5;
              break;
          }
        }

        if ((text_dimensions.bottom < -0.501f) || (text_dimensions.top > 0.501f))  //проверка горизонтального шрифта
        {
          switch (current_vertical_alignment)
          {
            case TextLineAlignment_Center:
              translate_vector.y = -(text_dimensions.top + text_dimensions.bottom) / 2.f;
              break;
            case TextLineAlignment_Top:
              translate_vector.y = -text_dimensions.top;
              break;
            case TextLineAlignment_Bottom:
              translate_vector.y = -text_dimensions.bottom;
              break;
          }
        }
        else
        {
          switch (current_vertical_alignment)
          {
            case TextLineAlignment_Top:
              translate_vector.y = -0.5;
              break;
            case TextLineAlignment_Bottom:
              translate_vector.y = 0.5;
              break;
          }
        }

        translate_tm = translate (translate_vector);
      }

        //обновление матрицы трансформаций
        
      math::mat4f world_tm = text_line->WorldTM ();

      size_t world_tm_hash = common::crc32 (&world_tm, sizeof (math::mat4f));

      if (need_update_position || (current_world_tm_hash != world_tm_hash))
      {
        primitive->SetTransform (world_tm * translate_tm);

        current_world_tm_hash = world_tm_hash;
        current_world_tm      = world_tm;
      }

      if (need_color_update)
      {
        current_color = text_line->Color ();

        render::mid_level::renderer2d::Sprite* current_sprite = sprites_buffer.data ();

        for (size_t i = 0; i < sprites_buffer.size (); i++, current_sprite++)
          current_sprite->color = current_color;     
      }

      if (need_rebuild_text || need_color_update)
      {
        primitive->RemoveAllSprites ();

        primitive->AddSprites (sprites_buffer.size (), sprites_buffer.data ());
      }
    }
    catch (std::exception& exception)
    {
      render.LogPrintf ("%s\n    at render::render2d::RenderableTextLine::Update(text_line='%s')", exception.what (), text_line->Name ());

      primitive->RemoveAllSprites ();
    }
    catch (...)
    {    
      render.LogPrintf ("Unknown exception\n    at render::render2d::RenderableTextLine::Update(text_line='%s')", text_line->Name ());    

      primitive->RemoveAllSprites ();
    }  
  }
};

/*
    Конструктор
*/

RenderableTextLine::RenderableTextLine (scene_graph::TextLine* text_line, Render& render)
  : Renderable (text_line), impl (new Impl (text_line, render))
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
  if (impl->primitive->GetSpritesCount ())
  {
    frame.AddPrimitive (impl->primitive.get ());
  }
}
