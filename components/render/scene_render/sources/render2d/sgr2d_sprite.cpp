#include "shared.h"

using namespace render::render2d;
using namespace scene_graph;

/*
    Конструктор
*/

RenderableSprite::RenderableSprite (scene_graph::Sprite* in_sprite, Render& in_render)
  : Renderable (in_sprite),
    render (in_render),
    sprite (in_sprite),
    primitive (render.Renderer ()->CreatePrimitive (), false)
{
  RenderableSprite::UpdateNotify ();
}

/*
    Оповещение об обновлении параметров scene_graph::Sprite
*/

void RenderableSprite::UpdateNotify ()
{  
    //оптимизировать в расчёте на частичное изменение параметров!!!

  try
  {
      //получение ресурсов из кэша      

    SpriteMaterial* material = render.GetMaterial (sprite->Material ());
    ITexture*       texture  = render.GetTexture (material->Image ());
    
      //преобразование данных
    
    BlendMode blend_mode;    
    
    switch (material->BlendMode ())
    {
      default:
      case media::rfx::SpriteBlendMode_None:        blend_mode = render::mid_level::renderer2d::BlendMode_None; break;
      case media::rfx::SpriteBlendMode_Translucent: blend_mode = render::mid_level::renderer2d::BlendMode_Translucent; break;
      case media::rfx::SpriteBlendMode_Mask:        blend_mode = render::mid_level::renderer2d::BlendMode_Mask; break;
      case media::rfx::SpriteBlendMode_Additive:    blend_mode = render::mid_level::renderer2d::BlendMode_Additive; break;
    }
    
      //установка параметров примитива
    
    primitive->SetBlendMode (blend_mode);
    primitive->SetTexture   (texture);
    primitive->SetTransform (sprite->WorldTM ());
    
      //добавление спрайта
      
    render::mid_level::renderer2d::Sprite s;
    
    s.position = math::vec3f (0.0f);
    s.size     = math::vec2f (1.0f);
    s.color    = math::vec4f (sprite->Color (), sprite->Alpha ());
    
    if (material->IsTiled ())
    {
      size_t texture_width  = texture->GetWidth (),
             texture_height = texture->GetHeight (),      
             frame          = sprite->Frame ();
             
      if (!texture_width || !texture_height)
        throw xtl::format_operation_exception ("", "Bad texture '%s' (width=%u, height=%u)", material->Image (), texture_width,
          texture_height);
          
      size_t tile_width  = material->TileWidth (),
             tile_height = material->TileHeight ();

      if (!tile_width || !tile_height)
        throw xtl::format_operation_exception ("", "Bad material '%s' (tile_width=%u, tile_height=%u)", sprite->Material (),
          tile_width, tile_height);

      size_t tile_columns = texture_width / tile_width;

      if (!tile_columns)
        tile_columns = 1;

      size_t tile_row    = frame / tile_columns,
             tile_column = frame % tile_columns;

      s.tex_offset = math::vec2f (tile_column * tile_width / float (texture_width),
                                  tile_row * tile_height / float (texture_height));
      s.tex_size   = math::vec2f (tile_width / float (texture_width), tile_height / float (texture_height));
    }
    else
    {
      s.tex_offset = math::vec2f (0.0f);
      s.tex_size   = math::vec2f (1.0f);
    }

    primitive->AddSprites (1, &s);
  }
  catch (std::exception& exception)
  {
    render.LogPrintf ("%s\n    at render::render2d::RenderableSprite::UpdateNotify(sprite='%s')", exception.what (), sprite->Name ());

    primitive->RemoveAllSprites ();
  }
  catch (...)
  {    
    render.LogPrintf ("Unknown exception\n    at render::render2d::RenderableSprite::UpdateNotify(sprite='%s')", sprite->Name ());    

    primitive->RemoveAllSprites ();
  }  
}

/*
    Рисование
*/

void RenderableSprite::Draw (IFrame& frame)
{
  if (primitive->GetSpritesCount ())
  {
    frame.AddPrimitive (primitive.get ());
  }
}
