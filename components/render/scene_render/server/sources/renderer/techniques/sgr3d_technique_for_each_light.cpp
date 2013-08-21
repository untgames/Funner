#include "../shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    Константы
*/

const size_t LIGHTS_RESERVE_SIZE = 64;   //резервируемое количество источников света

/*
    Настройки
*/

struct Settings
{
//  stl::string first_light_effect;
//  stl::string others_light_effect;
};

}

/*
    Описание реализации техники
*/

typedef stl::vector<Frame> FrameArray;

struct ForEachLightTechnique::Impl
{
  RenderManager             manager;             //менеджер рендеринга
  FrameArray                light_frames;        //фреймы источников света  
  Frame::EntityDrawFunction entity_draw_handler; //обработчик отрисовки объектов
  Log                       log;                 //протокол вывода
  
///Конструктор
  Impl (RenderManager& in_manager, common::ParseNode& cfg_node)
    : manager (in_manager)
    , entity_draw_handler (xtl::bind (&Impl::EntityDrawHandler, this, _1, _2, _3))
  {
    light_frames.reserve (LIGHTS_RESERVE_SIZE);    
  }    
  
///Резервирование фрейма
  Frame& AllocateFrame (size_t light_index)
  {
    if (light_index < light_frames.size ())
      return light_frames [light_index];    
    
    Frame frame = manager.CreateFrame ();
    
    frame.SetEntityDrawHandler (entity_draw_handler);
      
    light_frames.push_back (frame);
      
    return light_frames.back ();
  }
  
///Обновление кадра из источника света
  void UpdateFromLight (Scene& scene, Frame& parent_frame, Light& light, TraverseResult& result, size_t light_index)
  {
      //резервирование вложенного фрейма
      
    Frame& frame = AllocateFrame (light_index);

      //установка параметров вложенного фрейма
      
    //TODO: configure frame
    
      //обновление визуализируемых объектов

    for (TraverseResult::RenderableArray::iterator iter=result.renderables.begin (), end=result.renderables.end (); iter!=end; ++iter)
    {
      Renderable& renderable = **iter;
      
      renderable.UpdateFrame (frame);
    }

      //добавление вложенного фрейма в родительский

    parent_frame.AddFrame (frame);
  }
  
///Обработчик отрисовки объектов
  void EntityDrawHandler (Frame& frame, Entity& entity, EntityDrawParams& out_params)
  {
//    common::PropertyMap& properties = out_params.properties;
    
//    math::mat4f model_view_tm = frame.Properties ().GetMatrix ("ViewMatrix") * entity.Properties ().GetMatrix ("ObjectMatrix");  

//    out_params.mvp_matrix = frame.Properties ().GetMatrix ("ProjectionMatrix") * model_view_tm;

//    properties.SetProperty ("ModelViewMatrix", model_view_tm);
//    properties.SetProperty ("ModelViewProjectionMatrix", out_params.mvp_matrix);    
  }
};

/*
    Конструктор / деструктор
*/

ForEachLightTechnique::ForEachLightTechnique (RenderManager& manager, common::ParseNode& node)
  : impl (new Impl (manager, node))
{
  SetDefaultProperties (node);
}

ForEachLightTechnique::~ForEachLightTechnique ()
{
}

/*
    Обновление кадра
*/

void ForEachLightTechnique::UpdateFrameCore (Scene& scene, Frame& frame, ITraverseResultCache& traverse_result_cache)
{
  try
  {
    if (!Camera ())
      return;      
      
      //получение камеры

    scene_graph::Camera& camera = *Camera ();

      //определение списка источников и визуализируемых объектов, попадающих в камеру
    
    TraverseResult& result = traverse_result_cache.Result ();

      //обход источников
      
    size_t light_index = 0;

    for (TraverseResult::LightArray::iterator iter=result.lights.begin (), end=result.lights.end (); iter!=end; ++iter, ++light_index)
      impl->UpdateFromLight (scene, frame, **iter, result, light_index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::ForEachLightTechnique::UpdateFrameCore");
    throw;
  }
}

/*
    Обновление свойств
*/

void ForEachLightTechnique::ResetPropertiesCore ()
{
  impl->log.Printf ("%s", __FUNCTION__);
}

void ForEachLightTechnique::UpdatePropertiesCore (const common::PropertyMap& properties)
{
  impl->log.Printf ("%s:", __FUNCTION__);
  
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {
    const char* name = properties.PropertyName (i);
    
    stl::string value;
    
    properties.GetProperty (name, value);
    
    impl->log.Printf ("  %s=%s", name, value.c_str ());
  }
}
