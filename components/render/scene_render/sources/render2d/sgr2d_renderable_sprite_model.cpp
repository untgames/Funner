#include "shared.h"

using namespace render::render2d;
using namespace scene_graph;

/*
    Конструктор
*/

RenderableSpriteModel::RenderableSpriteModel (scene_graph::SpriteModel* in_model, Render& in_render)
  : Renderable (in_model),
    render (in_render),
    model (in_model),
    primitive (render.Renderer ()->CreatePrimitive (), false)
{
}

/*
    Оповещение об обновлении параметров scene_graph::Sprite
*/

void RenderableSpriteModel::Update ()
{  
  primitive->RemoveAllSprites ();

    //оптимизировать в расчёте на частичное изменение параметров!!!

  try
  {
      //получение ресурсов из кэша

    SpriteMaterial* material = render.GetMaterial (model->Material ());
    ITexture*       texture  = render.GetTexture (material->Image ());
    
      //проверка корректности данных - получение размеров текстуры и тайла
      
    bool   is_tiled = material->IsTiled ();
    size_t tile_columns;
    float  tile_tex_width, tile_tex_height;
      
    if (is_tiled)
    {        
      size_t texture_width  = texture->GetWidth (),
             texture_height = texture->GetHeight ();

      if (!texture_width || !texture_height)
        throw xtl::format_operation_exception ("", "Bad texture '%s' (width=%u, height=%u)", material->Image (), texture_width,
          texture_height);
          
      size_t tile_width  = material->TileWidth (),
             tile_height = material->TileHeight ();

      if (!tile_width || !tile_height)
        throw xtl::format_operation_exception ("", "Bad material '%s' (tile_width=%u, tile_height=%u)", model->Material (),
          tile_width, tile_height);
          
      tile_columns = texture_width / tile_width;
      
      if (!tile_columns)
        tile_columns = 1;
        
      tile_tex_width = tile_width / float (texture_width);
      tile_tex_height = tile_height / float (texture_height);
    }

      //преобразование режима смешивания цветов

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
    primitive->SetTransform (model->WorldTM ());

      //добавление спрайтов
      
    size_t sprites_count = model->SpriteDescsCount ();
      
    primitive->ReserveSprites (sprites_count);
      
    const scene_graph::SpriteModel::SpriteDesc* src_sprite = model->SpriteDescs ();
      
    for (size_t i=0; i<sprites_count; i++, src_sprite++)
    {
      render::mid_level::renderer2d::Sprite dst_sprite;

      dst_sprite.position = src_sprite->position;
      dst_sprite.size     = src_sprite->size;
      dst_sprite.color    = src_sprite->color;

      if (is_tiled)
      {
        size_t frame       = src_sprite->frame,
               tile_row    = frame / tile_columns,
               tile_column = frame % tile_columns;

        dst_sprite.tex_offset = math::vec2f (tile_column * tile_tex_width, tile_row * tile_tex_height);
        dst_sprite.tex_size   = math::vec2f (tile_tex_width, tile_tex_height);
      }
      else
      {
        dst_sprite.tex_offset = math::vec2f (0.0f);
        dst_sprite.tex_size   = math::vec2f (1.0f);
      }

      primitive->AddSprites (1, &dst_sprite);
    }      
  }
  catch (std::exception& exception)
  {
    render.LogPrintf ("%s\n    at render::render2d::RenderableSpriteModel::UpdateNotify(model='%s')", exception.what (), model->Name ());

    primitive->RemoveAllSprites ();
  }
  catch (...)
  {    
    render.LogPrintf ("Unknown exception\n    at render::render2d::RenderableSpriteModel::UpdateNotify(model='%s')", model->Name ());    

    primitive->RemoveAllSprites ();
  }  
}

/*
    Рисование
*/

void RenderableSpriteModel::DrawCore (IFrame& frame)
{
  if (primitive->GetSpritesCount ())
  {
    frame.AddPrimitive (primitive.get ());
  }
}
