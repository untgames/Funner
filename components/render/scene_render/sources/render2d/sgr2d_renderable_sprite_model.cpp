#include "shared.h"

using namespace render::render2d;
using namespace scene_graph;

/*
    Описание реализации спрайтовой модели
*/

struct RenderableSpriteModel::Impl: public xtl::trackable
{
  Render&                   render;                          //ссылка на рендер
  scene_graph::SpriteModel* model;                           //исходная модель
  PrimitivePtr              primitive;                       //визуализируемый примитив
  RenderQueryPtr            query;                           //запрос дочернего рендеринга
  bool                      need_update_sprites;             //флаг необходимости обновления массива спрайтов
  size_t                    tile_columns, tile_rows;         //количество столбцов / строк тайлов
  float                     tile_tex_width, tile_tex_height; //размеры тайла в текстурных координатах
  size_t                    current_world_tm_hash;           //хэш текущей матрицы трансформации
  size_t                    current_material_name_hash;      //хэш текущего имени материала
  float                     current_alpha_reference;         //текущее значение параметра альфа-отсечения

///Конструктор
  Impl (scene_graph::SpriteModel* in_model, Render& in_render) :
    render (in_render),
    model (in_model),
    primitive (render.Renderer ()->CreatePrimitive (), false),
    need_update_sprites (true),
    current_world_tm_hash (0),
    current_material_name_hash (0)  
  {
    connect_tracker (model->RegisterEventHandler (SpriteModelEvent_AfterSpriteDescsUpdate, xtl::bind (&Impl::UpdateSpritesNotify, this)));

    current_alpha_reference = primitive->GetAlphaReference ();
  }
  
///Оповещение об обновлении спрайтов 
  void UpdateSpritesNotify ()
  {
    need_update_sprites = true;
  }
};

/*
    Конструктор
*/

RenderableSpriteModel::RenderableSpriteModel (scene_graph::SpriteModel* in_model, Render& in_render)
  : Renderable (in_model),
    impl (new Impl (in_model, in_render))
{
}

RenderableSpriteModel::~RenderableSpriteModel ()
{
}

/*
    Оповещения об обновлении модели
*/

void RenderableSpriteModel::Update ()
{
    //получение текущей модели
     
  scene_graph::SpriteModel* model = impl->model;

  try
  {
      //обновление материала

    size_t material_name_hash = common::strhash (model->Material ());

    if (material_name_hash != impl->current_material_name_hash)
    {
        //получение материала из кэша

      SpriteMaterial* material = impl->render.GetMaterial (model->Material ());      
      
        //преобразование режима смешивания цветов

      BlendMode blend_mode;
      bool      need_alpha;

      switch (material->BlendMode ())
      {
        default:
        case media::rfx::SpriteBlendMode_None:
          blend_mode = render::mid_level::renderer2d::BlendMode_None;
          need_alpha = false;
          break;
        case media::rfx::SpriteBlendMode_Translucent:
          blend_mode = render::mid_level::renderer2d::BlendMode_Translucent;
          need_alpha = false;
          break;
        case media::rfx::SpriteBlendMode_Mask:
          blend_mode = render::mid_level::renderer2d::BlendMode_Mask;
          need_alpha = false;
          break;
        case media::rfx::SpriteBlendMode_Additive:
          blend_mode = render::mid_level::renderer2d::BlendMode_Additive;
          need_alpha = false;
          break;
        case media::rfx::SpriteBlendMode_AlphaClamp:
          blend_mode = render::mid_level::renderer2d::BlendMode_AlphaClamp;
          need_alpha = true;
          break;
      }

        //получение текстуры из кэша
        
      ITexture* texture = impl->render.GetTexture (material->Image (), need_alpha, impl->query);
      
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

        impl->tile_columns = texture_width / tile_width;
        impl->tile_rows    = texture_height / tile_height;

        if (!impl->tile_columns)
          impl->tile_columns = 1;
          
        if (!impl->tile_rows)
          impl->tile_rows = 1;
          
        impl->tile_tex_width  = tile_width / float (texture_width);
        impl->tile_tex_height = tile_height / float (texture_height);
      }
      else
      {
          //маркер отсутствия тайлинга        
        
        impl->tile_columns = 0; 
        impl->tile_rows    = 0;
      }
      
        //установка параметров примитива

      impl->primitive->SetBlendMode (blend_mode);
      impl->primitive->SetTexture   (texture);      

      impl->current_material_name_hash = material_name_hash;
    }

    if (impl->current_alpha_reference != model->AlphaReference ())
    {
      impl->primitive->SetAlphaReference (model->AlphaReference ());

      impl->current_alpha_reference = model->AlphaReference ();
    }
    
      //обновление матрицы трансформаций

    math::mat4f world_tm;

    model->EvalWorldTMAfterPivot (world_tm);

    size_t world_tm_hash = common::crc32 (&world_tm, sizeof (math::mat4f));

    if (impl->current_world_tm_hash != world_tm_hash)
    {
      impl->primitive->SetTransform (world_tm);

      impl->current_world_tm_hash = world_tm_hash;
    }    

      //обновление массива спрайтов
    
    if (impl->need_update_sprites)
    {
      impl->primitive->RemoveAllSprites ();

      size_t sprites_count = model->SpriteDescsCount ();

      impl->primitive->ReserveSprites (sprites_count);

      const scene_graph::SpriteModel::SpriteDesc* src_sprite = model->SpriteDescs ();
        
      for (size_t i=0; i<sprites_count; i++, src_sprite++)
      {
        render::mid_level::renderer2d::Sprite dst_sprite;

        dst_sprite.position = src_sprite->position;
        dst_sprite.size     = src_sprite->size;
        dst_sprite.color    = src_sprite->color;

        if (impl->tile_columns || impl->tile_rows)
        {
          size_t frame       = src_sprite->frame,
                 tile_row    = frame / impl->tile_columns % impl->tile_rows,
                 tile_column = frame % impl->tile_columns;

//          dst_sprite.tex_offset = math::vec2f (tile_column * impl->tile_tex_width, tile_row * impl->tile_tex_height);
          dst_sprite.tex_offset = math::vec2f (tile_column * impl->tile_tex_width, 1.0f - tile_row * impl->tile_tex_height - impl->tile_tex_height);
          dst_sprite.tex_size   = math::vec2f (impl->tile_tex_width, impl->tile_tex_height);
        }
        else
        {
          dst_sprite.tex_offset = math::vec2f (0.0f);
          dst_sprite.tex_size   = math::vec2f (1.0f);
        }

        impl->primitive->AddSprites (1, &dst_sprite);
      }
      
      impl->need_update_sprites = false;
    }      
  }
  catch (std::exception& exception)
  {
    impl->render.LogPrintf ("%s\n    at render::render2d::RenderableSpriteModel::Update(model='%s')", exception.what (), model->Name ());

    impl->primitive->RemoveAllSprites ();
  }
  catch (...)
  {    
    impl->render.LogPrintf ("Unknown exception\n    at render::render2d::RenderableSpriteModel::Update(model='%s')", model->Name ());    

    impl->primitive->RemoveAllSprites ();
  }  
}

/*
    Рисование
*/

void RenderableSpriteModel::DrawCore (IFrame& frame)
{
  if (impl->primitive->GetSpritesCount ())
  {
    if (impl->query)
      impl->query->Update ();    

    frame.AddPrimitive (impl->primitive.get ());
  }
}
