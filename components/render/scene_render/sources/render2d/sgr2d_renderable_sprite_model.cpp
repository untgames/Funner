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
    primitive (render.Renderer ()->CreatePrimitive (), false),
    need_update_material (true),
    need_update_sprites (true),
    current_world_tm_hash (0)
{
  //добавить реакцию на пивоты!!!
  connect_tracker (model->RegisterEventHandler (SpriteModelEvent_AfterSpriteDescsUpdate, xtl::bind (&RenderableSpriteModel::UpdateSpritesNotify, this)));
  connect_tracker (model->RegisterEventHandler (SpriteModelEvent_AfterMaterialUpdate, xtl::bind (&RenderableSpriteModel::UpdateMaterialNotify, this)));
}

RenderableSpriteModel::~RenderableSpriteModel ()
{
}

/*
    Оповещения об обновлении модели
*/

void RenderableSpriteModel::UpdateMaterialNotify ()
{
  need_update_material = true;
}

void RenderableSpriteModel::UpdateSpritesNotify ()
{
  need_update_sprites = true;
}

void RenderableSpriteModel::Update ()
{
  try
  {
      //обновление материала
    
    if (need_update_material)
    {
        //получение ресурсов из кэша

      SpriteMaterial* material = render.GetMaterial (model->Material ());
      ITexture*       texture  = render.GetTexture (material->Image ());
      
        //проверка корректности данных - получение размеров текстуры и тайла

      if (material->IsTiled ())
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
          
        tile_tex_width  = tile_width / float (texture_width);
        tile_tex_height = tile_height / float (texture_height);
      }
      else
      {
        tile_columns = 0; //маркер отсутствия тайлинга
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
        case media::rfx::SpriteBlendMode_AlphaClamp:  blend_mode = render::mid_level::renderer2d::BlendMode_AlphaClamp; break;
      }

      primitive->SetBlendMode (blend_mode);
      primitive->SetTexture   (texture);

      need_update_material = false;
    }
    
      //обновление матрицы трансформаций
      
    math::mat4f world_tm;

    model->EvalWorldTMAfterPivot (world_tm);

    size_t world_tm_hash = common::crc32 (&world_tm, sizeof (math::mat4f));

    if (current_world_tm_hash != world_tm_hash)
    {
      primitive->SetTransform (world_tm);

      current_world_tm_hash = world_tm_hash;
    }

      //обновление массива спрайтов
    
    if (need_update_sprites)
    {
      primitive->RemoveAllSprites ();

      size_t sprites_count = model->SpriteDescsCount ();

      primitive->ReserveSprites (sprites_count);
        
      const scene_graph::SpriteModel::SpriteDesc* src_sprite = model->SpriteDescs ();
        
      for (size_t i=0; i<sprites_count; i++, src_sprite++)
      {
        render::mid_level::renderer2d::Sprite dst_sprite;

        dst_sprite.position = src_sprite->position;
        dst_sprite.size     = src_sprite->size;
        dst_sprite.color    = src_sprite->color;

        if (tile_columns)
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
      
      need_update_sprites = false;
    }      
  }
  catch (std::exception& exception)
  {
    render.LogPrintf ("%s\n    at render::render2d::RenderableSpriteModel::Update(model='%s')", exception.what (), model->Name ());

    primitive->RemoveAllSprites ();
  }
  catch (...)
  {    
    render.LogPrintf ("Unknown exception\n    at render::render2d::RenderableSpriteModel::Update(model='%s')", model->Name ());    

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
