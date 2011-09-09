#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    Фабрика объектов
*/

struct EntityFactory: public xtl::visitor<Node*, scene_graph::VisualModel>
{
  Scene& scene;  
  
  EntityFactory (Scene& in_scene) : scene (in_scene) {}
  
  Node* visit (scene_graph::VisualModel& entity)
  {
    try
    {
      return new VisualModel (scene, entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::visit(scene_graph::VisualModel&)");
      throw;
    }
  }
  
  Node* CreateNode (scene_graph::Entity& entity)
  {
    try
    {
      return (*this)(entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::CreateNode");
      throw;
    }
  }
};

}

/*
    Описание реализации сцены рендеринга
*/

typedef stl::hash_map<scene_graph::Entity*, NodePtr> EntityMap;

struct Scene::Impl
{
  RenderPtr            render;                     //рендер
  scene_graph::Scene*  scene;                      //исходная сцена
  xtl::auto_connection scene_destroyed_connection; //соединение с обработчиком события удаления сцены
  EntityMap            entities;                   //карта объектов
  EntityFactory        factory;                    //фабрика объектов
  Log                  log;                        //поток протоколирования

///Конструктор
  Impl (Scene& owner, scene_graph::Scene& in_scene, const RenderPtr& in_render)
    : render (in_render)
    , scene (&in_scene)
    , scene_destroyed_connection (in_scene.Root ().RegisterEventHandler (scene_graph::NodeEvent_BeforeDestroy, xtl::bind (&Impl::OnDestroyScene, this)))
    , factory (owner)
  {
    log.Printf ("Scene created");
  }
  
///Деструктор
  ~Impl ()
  {
    log.Printf ("Scene destroyed");
  }
  
///Оповещение об удалении сцены
  void OnDestroyScene ()
  {
    if (scene)
      render->UnregisterScene (*scene);    
      
    scene = 0;
  }  
};

/*
    Конструктор / деструктор
*/

Scene::Scene (scene_graph::Scene& scene, const RenderPtr& render)
{
  try
  {
    if (!render)
      throw xtl::make_null_argument_exception ("", "render");    
    
    impl = new Impl (*this, scene, render);
    
    render->RegisterScene (scene, *this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Scene::Scene");
    throw;
  }
}

Scene::~Scene ()
{
  try
  {
    if (impl->scene)
      impl->render->UnregisterScene (*impl->scene);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Регистрация объектов
*/

void Scene::RegisterEntity (scene_graph::Entity& scene_node, Node& node)
{
  impl->entities.insert_pair (&scene_node, &node);
}

void Scene::UnregisterEntity (scene_graph::Entity& scene_node)
{
  impl->entities.erase (&scene_node);
}

/*
    Получение объекта сцены
*/

NodePtr Scene::GetEntity (scene_graph::Entity& entity)
{
  try
  {
    EntityMap::iterator iter = impl->entities.find (&entity);
    
    if (iter != impl->entities.end ())
      return iter->second;
      
    return NodePtr (impl->factory.CreateNode (entity), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Scene::GetEntity");
    throw;
  }
}
