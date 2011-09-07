#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации визуализируемого объекта
*/

struct Renderable::Impl
{
  Scene&               scene;                        //сцена, которой принадлежит объект
  scene_graph::Entity* entity;                       //исходный объект
  xtl::auto_connection on_entity_destroy_connection; //оповещение об удалении объекта
  xtl::auto_connection on_entity_update_connection;  //оповещение об обновлении объекта
  bool                 need_update;                  //объект требует обновления

///Конструктор
  Impl (Scene& in_scene, scene_graph::Entity& in_entity)
    : scene (in_scene)
    , entity (&in_entity)
    , on_entity_destroy_connection (entity->RegisterEventHandler (scene_graph::NodeEvent_BeforeDestroy, xtl::bind (&Impl::OnDestroyEntity, this)))
    , on_entity_update_connection (entity->RegisterEventHandler (scene_graph::NodeEvent_AfterUpdate, xtl::bind (&Impl::OnUpdateEntity, this)))
    , need_update (true)
  {
  }

///Оповещение об удалении объекта
  void OnDestroyEntity ()
  {
    if (entity)
      scene.UnregisterRenderable (*entity);
      
    entity = 0;
  }

///Оповещение об обновлении объекта
  void OnUpdateEntity ()
  {
    need_update = true;
  }
};

/*
    Конструктор / деструктор
*/

Renderable::Renderable (Scene& scene, scene_graph::Entity& entity)
{
  try  
  {    
    impl = new Impl (scene, entity);
    
    scene.RegisterRenderable (entity, *this);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Renderable::Renderable");
    throw;
  }
}

Renderable::~Renderable ()
{
  try
  {
    if (impl->entity)
      impl->scene.UnregisterRenderable (*impl->entity);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Отрисовка
*/

void Renderable::Draw (Frame& frame)
{
  if (!impl->entity)
    return;

  if (impl->need_update)
  {
    UpdateCore (frame);

    impl->need_update = false;
  }

  DrawCore (frame);  
}
