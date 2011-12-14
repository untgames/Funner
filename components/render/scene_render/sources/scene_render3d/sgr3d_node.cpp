#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации визуализируемого объекта
*/

struct Node::Impl
{
  Scene&               scene;                        //сцена, которой принадлежит объект
  scene_graph::Entity* entity;                       //исходный объект
  xtl::auto_connection on_entity_destroy_connection; //оповещение об удалении объекта
  xtl::auto_connection on_entity_update_connection;  //оповещение об обновлении объекта
  bool                 need_update;                  //объект требует обновления
  math::mat4f          world_tm;                     //мировая матрица преобразований  

///Конструктор
  Impl (Scene& in_scene, scene_graph::Entity& in_entity)
    : scene (in_scene)
    , entity (&in_entity)
    , on_entity_destroy_connection (entity->RegisterEventHandler (scene_graph::NodeEvent_BeforeDestroy, xtl::bind (&Impl::OnDestroyEntity, this)))
    , on_entity_update_connection (entity->RegisterEventHandler (scene_graph::NodeEvent_AfterUpdate, xtl::bind (&Impl::OnUpdateEntity, this)))
    , need_update (true)
    , world_tm (in_entity.WorldTM ())
  {
  }

///Оповещение об удалении объекта
  void OnDestroyEntity ()
  {
    if (entity)
      scene.UnregisterEntity (*entity);
      
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

Node::Node (Scene& scene, scene_graph::Entity& entity)
{
  try  
  {    
    impl = new Impl (scene, entity);
    
    scene.RegisterEntity (entity, *this);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Node::Node");
    throw;
  }
}

Node::~Node ()
{
  try
  {
    if (impl->entity)
      impl->scene.UnregisterEntity (*impl->entity);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Обход
*/

void Node::Visit (IVisitor& visitor)
{
  VisitCore (visitor);
}

void Node::VisitCore (IVisitor& visitor)
{
  visitor.Visit (*this);
}

/*
    Обновление состояния
*/

void Node::Update ()
{
  if (!impl->entity)
    return;

  if (impl->need_update)
  {
    impl->world_tm = impl->entity->WorldTM ();
    
    UpdateCore ();    

    impl->need_update = false;
  }
}

void Node::UpdateCore ()
{
}

/*
    Положение в пространстве
*/

const math::mat4f& Node::WorldTM ()
{
  Update ();

  return impl->world_tm;
}
