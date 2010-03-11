#include "shared.h"

using namespace render::render2d;
using namespace scene_graph;

/*
    Описание реализации спрайтовой модели
*/

struct RenderableSpriteModel::Impl: public xtl::trackable
{
  Render&                     render;                          //ссылка на рендер
  scene_graph::SpriteModel*   model;                           //исходная модель
  PrimitivePtr                primitive;                       //визуализируемый примитив
  RenderQueryPtr              query;                           //запрос дочернего рендеринга
  bool                        need_update_sprites;             //флаг необходимости обновления массива спрайтов
  size_t                      tile_columns, tile_rows;         //количество столбцов / строк тайлов
  float                       tile_tex_width, tile_tex_height; //размеры тайла в текстурных координатах
  math::vec3f                 tile_tex_offset;                 //смещение от начала текстуры
  size_t                      current_world_tm_hash;           //хэш текущей матрицы трансформации
  size_t                      current_material_name_hash;      //хэш текущего имени материала
  float                       current_alpha_reference;         //текущее значение параметра альфа-отсечения
  math::vec2f                 current_texture_scroll;          //текущее смещение текстуры
  bool                        current_scissor_state;           //состояние области отсечения
  render::mid_level::Viewport current_scissor_rect;            //разрмеры области отсечения  
  size_t                      texture_scroll_property_index;   //текущее значение индекса свойства смещения текстурных координат
  size_t                      scissor_rect_property_index;     //текущее значение индекса свойства области отсечения
  size_t                      scissor_state_property_index;    //текущее значение индекса свойства состояния области отсечения
  size_t                      properties_structure_hash;       //текущий хэш структуры свойств узла
  size_t                      properties_hash;                 //текущий хэш свойств узла

///Конструктор
  Impl (scene_graph::SpriteModel* in_model, Render& in_render) :
    render (in_render),
    model (in_model),
    primitive (render.Renderer ()->CreatePrimitive (), false),
    need_update_sprites (true),
    current_world_tm_hash (0),
    current_material_name_hash (0),
    current_texture_scroll (0.0f),
    current_scissor_state (false),
    texture_scroll_property_index (0),
    scissor_rect_property_index (0),
    scissor_state_property_index (0),
    properties_structure_hash (0),
    properties_hash (0)
  {
    memset (&current_scissor_rect, 0, sizeof (current_scissor_rect));
    
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
      //обновление параметров рендеринга
      
    bool need_update_scissor = false;
    bool scissor_state       = impl->current_scissor_state;
    render::mid_level::Viewport scissor_rect = {0, 0, 0, 0};
      
    if (!model->Properties () && impl->properties_hash)
    {
      impl->properties_hash               = 0;
      impl->properties_structure_hash     = 0;
      impl->texture_scroll_property_index = 0;
      impl->scissor_rect_property_index   = 0;
      impl->scissor_state_property_index  = 0;
      impl->current_texture_scroll        = math::vec2f (0.0f);
      impl->need_update_sprites           = true;
      need_update_scissor                 = true;
      scissor_state                       = false;
    }
      
    if (model->Properties () && impl->properties_hash != model->Properties ()->Hash ())
    {     
      const NodeProperties& properties = *model->Properties ();
      
      if (impl->properties_structure_hash != properties.StructureHash ())
      {
        impl->texture_scroll_property_index = properties.IndexOf ("render.texture_scroll");
        impl->scissor_rect_property_index   = properties.IndexOf ("render.scissor_rect");
        impl->scissor_state_property_index  = properties.IndexOf ("render.scissor_state");
        impl->properties_structure_hash     = properties.StructureHash ();
      }

      if (impl->texture_scroll_property_index != size_t (-1))
      {
        math::vec4f scroll;

        try
        {
          properties.GetProperty (impl->texture_scroll_property_index, scroll);

          impl->current_texture_scroll = scroll;
        }
        catch (...)
        {
          impl->current_texture_scroll = math::vec2f (0.0f);
        }
        
        impl->need_update_sprites = true;        
      }
      else
      {
        if (impl->current_texture_scroll != math::vec2f (0.0f))
          impl->need_update_sprites = true;
        
        impl->current_texture_scroll = math::vec2f (0.0f);
      }
      
      if (impl->scissor_state_property_index != size_t (-1))
      {
        if (impl->scissor_rect_property_index != size_t (-1))
        {
          try
          {
            scissor_state = properties.GetInteger (impl->scissor_state_property_index) != 0;
          }
          catch (...)
          {
            scissor_state = false;
          }
        }
        else
        {
          scissor_state = false;
        }
        
        need_update_scissor = true;
      }
      else
      {
        if (scissor_state)
          need_update_scissor = true;
          
        scissor_state = false;
      }

      if (impl->scissor_rect_property_index != size_t (-1))
      {
        math::vec4f rect;
        
        try
        {
          properties.GetProperty (impl->scissor_rect_property_index, rect);
          
          scissor_rect.x      = (int)rect [0];
          scissor_rect.y      = (int)rect [1];
          scissor_rect.width  = (size_t)rect [2];
          scissor_rect.height = (size_t)rect [3];

          if (impl->scissor_state_property_index == size_t (-1))
            scissor_state = true;
        }
        catch (...)
        {
          scissor_state = false;
        }
        
        need_update_scissor = true;
      }
      else
      {
        if (scissor_state)
          need_update_scissor = true;
        
        scissor_state = false;
      }
      
      impl->properties_hash = properties.Hash ();
    }    
    
      //обновление материала

    size_t material_name_hash = common::strhash (model->Material ());

    if (material_name_hash != impl->current_material_name_hash)
    {
        //получение материала из кэша

      SpriteMaterial* material = impl->render.GetMaterial (model->Material ());      
      
        //преобразование режима смешивания цветов

      BlendMode                                 blend_mode;
      render::mid_level::renderer2d::ShaderMode shader_mode;
      bool                                      need_alpha;

      switch (material->BlendMode ())
      {
        default:
        case media::rfx::SpriteBlendMode_None:
          blend_mode  = render::mid_level::renderer2d::BlendMode_None;
          shader_mode = render::mid_level::renderer2d::ShaderMode_Normal;
          need_alpha  = false;
          break;
        case media::rfx::SpriteBlendMode_Translucent:
          blend_mode  = render::mid_level::renderer2d::BlendMode_Translucent;
          shader_mode = render::mid_level::renderer2d::ShaderMode_Normal;
          need_alpha  = false;
          break;
        case media::rfx::SpriteBlendMode_Mask:
          blend_mode  = render::mid_level::renderer2d::BlendMode_Mask;
          shader_mode = render::mid_level::renderer2d::ShaderMode_Normal;
          need_alpha  = false;
          break;
        case media::rfx::SpriteBlendMode_Additive:
          blend_mode  = render::mid_level::renderer2d::BlendMode_Additive;
          shader_mode = render::mid_level::renderer2d::ShaderMode_Normal;
          need_alpha  = false;
          break;
        case media::rfx::SpriteBlendMode_AlphaClamp:
          blend_mode  = render::mid_level::renderer2d::BlendMode_Translucent;
          shader_mode = render::mid_level::renderer2d::ShaderMode_AlphaClamp;
          need_alpha  = true;
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
        impl->tile_tex_offset = math::vec2f (material->TileOffsetX () / float (texture_width), material->TileOffsetY () / float (texture_height));
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
    
    if (need_update_scissor)
    {
      if (impl->current_scissor_state != scissor_state)
      {
        impl->primitive->SetScissorState (scissor_state);
      }
      
      if (impl->current_scissor_rect.x != scissor_rect.x || impl->current_scissor_rect.y != scissor_rect.y ||
        impl->current_scissor_rect.width != scissor_rect.width || impl->current_scissor_rect.height != scissor_rect.height)
      {
        impl->primitive->SetScissor (scissor_rect);
      }
      
      impl->current_scissor_state = scissor_state;
      impl->current_scissor_rect  = scissor_rect;
    }
    
      //обновление матрицы трансформаций

    const math::mat4f& world_tm = model->WorldTM ();

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

          dst_sprite.tex_offset = math::vec2f (tile_column * impl->tile_tex_width, 1.0f - tile_row * impl->tile_tex_height - impl->tile_tex_height) + impl->current_texture_scroll + impl->tile_tex_offset;
          dst_sprite.tex_size   = math::vec2f (impl->tile_tex_width, impl->tile_tex_height);
        }
        else
        {
          dst_sprite.tex_offset = impl->current_texture_scroll + impl->tile_tex_offset;
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
