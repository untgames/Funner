#include "shared.h"

using namespace render::render2d;
using namespace scene_graph;

/*
    Описание реализации рендеринга карты высот
*/

struct RenderableHeightMap::Impl: public xtl::trackable
{
  Render&                     render;                          //ссылка на рендер
  scene_graph::HeightMap*     height_map;                      //исходная модель
  PrimitivePtr                primitive;                       //визуализируемый примитив
  RenderQueryPtr              query;                           //запрос дочернего рендеринга
  bool                        need_update_sprites;             //флаг необходимости обновления массива спрайтов
  size_t                      current_world_tm_hash;           //хэш текущей матрицы трансформации
  size_t                      current_material_name_hash;      //хэш текущего имени материала

///Конструктор
  Impl (scene_graph::HeightMap* in_height_map, Render& in_render) :
    render (in_render),
    height_map (in_height_map),
    primitive (render.Renderer ()->CreatePrimitive (), false),
    need_update_sprites (true),
    current_world_tm_hash (0),
    current_material_name_hash (0)
  {
    connect_tracker (height_map->RegisterEventHandler (scene_graph::HeightMapEvent_OnVerticesUpdate, xtl::bind (&Impl::UpdateNotify, this)));
  }

///Оповещение об обновлении карты
  void UpdateNotify ()
  {
    need_update_sprites = true;
  }
};

/*
    Конструктор
*/

RenderableHeightMap::RenderableHeightMap (scene_graph::HeightMap* in_height_map, Render& in_render)
  : Renderable (in_height_map),
    impl (new Impl (in_height_map, in_render))
{
}

RenderableHeightMap::~RenderableHeightMap ()
{
}

/*
    Оповещения об обновлении модели
*/

void RenderableHeightMap::Update ()
{
    //получение текущей модели
     
  scene_graph::HeightMap* height_map = impl->height_map;

  try
  {
      //обновление материала

    size_t material_name_hash = common::strhash (height_map->Material ());

    if (material_name_hash != impl->current_material_name_hash)
    {
        //получение материала из кэша

      SpriteMaterial* material = impl->render.GetMaterial (height_map->Material ());
      
        //преобразование режима смешивания цветов

      BlendMode                                 blend_mode;
      bool                                      need_alpha;

      switch (material->BlendMode ())
      {
        default:
        case media::rfx::SpriteBlendMode_None:
          blend_mode  = render::mid_level::renderer2d::BlendMode_None;
          need_alpha  = false;
          break;
        case media::rfx::SpriteBlendMode_Translucent:
          blend_mode  = render::mid_level::renderer2d::BlendMode_Translucent;
          need_alpha  = false;
          break;
        case media::rfx::SpriteBlendMode_Mask:
          blend_mode  = render::mid_level::renderer2d::BlendMode_Mask;
          need_alpha  = false;
          break;
        case media::rfx::SpriteBlendMode_Additive:
          blend_mode  = render::mid_level::renderer2d::BlendMode_Additive;
          need_alpha  = false;
          break;
        case media::rfx::SpriteBlendMode_AlphaClamp:
          blend_mode  = render::mid_level::renderer2d::BlendMode_Translucent;
          need_alpha  = true;
          break;
      }

        //получение текстуры из кэша
        
      ITexture* texture = impl->render.GetTexture (material->Image (), need_alpha, impl->query);
      
        //установка параметров примитива

      impl->primitive->SetBlendMode  (blend_mode);
      impl->primitive->SetShaderMode (render::mid_level::renderer2d::ShaderMode_Reflection); //?????
      impl->primitive->SetTexture    (texture);

      impl->current_material_name_hash = material_name_hash;
    }
    
      //обновление матрицы трансформаций

    const math::mat4f& world_tm = height_map->WorldTM ();

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

      size_t cells_count = height_map->RowsCount () * height_map->ColumnsCount ();

      impl->primitive->ReserveSprites (cells_count);
      
      stl::vector<render::mid_level::renderer2d::SpriteVertex> dst_vertices (cells_count * 4);

      const scene_graph::HeightMap::VertexDesc*    src_vertex = height_map->Vertices ();
      render::mid_level::renderer2d::SpriteVertex* dst_vertex = &dst_vertices [0];
      
      int indices [4] = {0, 1, height_map->ColumnsCount (), height_map->ColumnsCount () + 1};
      
      float dx = 1.0f / height_map->ColumnsCount (),
            dy = 1.0f / height_map->RowsCount (),
            y  = -0.5f;
            
      float x_offset [4] = {0, dx, 0, dx},
            y_offset [4] = {0, 0, dy, dy};

      for (size_t row=0, rows_count=height_map->RowsCount (); row<rows_count-1; row++, y += dy)
      {
        float x = -0.5f;
        
        for (size_t column=0, columns_count=height_map->ColumnsCount (); column<columns_count-1; column++, src_vertex++, x += dx)
        {
          for (size_t j=0; j<4; j++, dst_vertex++)
          {
            const scene_graph::HeightMap::VertexDesc& v = src_vertex [indices [j]];
            
            dst_vertex [j].position  = math::vec3f (x + x_offset [j], y + y_offset [j], v.height);
            dst_vertex [j].normal    = v.normal;
            dst_vertex [j].color     = v.color;
            dst_vertex [j].tex_coord = math::vec2f (x + x_offset [j] + 0.5f, y + y_offset [j] + 0.5f);
          }
        }
      }
      
      impl->primitive->AddSprites (cells_count, &dst_vertices [0]);
      
      impl->need_update_sprites = false;
    }      
  }
  catch (std::exception& exception)
  {
    impl->render.LogPrintf ("%s\n    at render::render2d::RenderableHeightMap::Update(model='%s')", exception.what (), height_map->Name ());

    impl->primitive->RemoveAllSprites ();
  }
  catch (...)
  {    
    impl->render.LogPrintf ("Unknown exception\n    at render::render2d::RenderableHeightMap::Update(model='%s')", height_map->Name ());    

    impl->primitive->RemoveAllSprites ();
  }  
}

/*
    Рисование
*/

void RenderableHeightMap::DrawCore (IFrame& frame)
{
  if (impl->primitive->GetSpritesCount ())
  {
    if (impl->query)
      impl->query->Update ();    

    frame.AddPrimitive (impl->primitive.get ());
  }
}
