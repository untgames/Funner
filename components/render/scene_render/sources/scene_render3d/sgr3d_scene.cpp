#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;
using namespace render::scene_render3d;

      //TODO: защита от неизвестных типов объектов!!!!!!!!!!!!!!!!!

namespace
{

/*
    Фабрика объектов
*/

struct EntityFactory: public xtl::visitor<void, scene_graph::VisualModel, scene_graph::PointLight, scene_graph::DirectLight, scene_graph::SpotLight>
{
  Scene& scene;
  Node*  result;
  
  EntityFactory (Scene& in_scene) : scene (in_scene), result () {}
  
  void visit (scene_graph::PointLight& entity)
  {
    try
    {
      result = new Light (scene, entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::visit(scene_graph::PointLight&)");
      throw;
    }
  }  
  
  void visit (scene_graph::SpotLight& entity)
  {
    try
    {
      result = new Light (scene, entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::visit(scene_graph::SpotLight&)");
      throw;
    }
  }  
  
  void visit (scene_graph::DirectLight& entity)
  {
    try
    {
      result = new Light (scene, entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::visit(scene_graph::DirectLight&)");
      throw;
    }
  }  
  
  void visit (scene_graph::VisualModel& entity)
  {
    try
    {
      result = new VisualModel (scene, entity);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::EntityFactory::visit(scene_graph::VisualModel&)");
      throw;
    }
  }
  
  Node* CreateNode (scene_graph::Node& entity)
  {
    try
    {
      result = 0;
      
      entity.Accept (*this);
      
      return result;
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

typedef stl::hash_map<scene_graph::Node*, NodePtr> EntityMap;

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
    Менеджер рендеринга
*/

RenderManager& Scene::Manager ()
{
  return impl->render->Manager ();
}

/*
    Регистрация объектов
*/

void Scene::RegisterEntity (scene_graph::Node& scene_node, Node& node)
{
  impl->entities.insert_pair (&scene_node, &node);
}

void Scene::UnregisterEntity (scene_graph::Node& scene_node)
{
  impl->entities.erase (&scene_node);
}

/*
    Получение объекта сцены
*/

Node* Scene::GetEntity (scene_graph::Node& entity)
{
  try
  {
    EntityMap::iterator iter = impl->entities.find (&entity);
    
    if (iter != impl->entities.end ())
      return &*iter->second;
      
    NodePtr node (impl->factory.CreateNode (entity), false); //будет автоматически сохранен в карте entities    
    
    if (!node)
    {
      //TODO: защита от неизвестных типов объектов!!!!!!!!!!!!!!!!!
      
      return 0;
    }
    
    return &*node;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Scene::GetEntity");
    throw;
  }
}

/*
    Обход сцены
*/

namespace
{

struct SceneCollector: public scene_graph::INodeTraverser
{
  public:
    SceneCollector (Scene& in_scene, CollectionVisitor& in_visitor)
      : scene (in_scene)
      , visitor (in_visitor)
    {
    }
  
    void operator () (scene_graph::Node& scene_node)
    {
      Node* node = scene.GetEntity (scene_node);
      
      if (!node)
        return;                

      node->Visit (visitor);
    }
    
  private:
    Scene&             scene;
    CollectionVisitor& visitor;
};

}

void Scene::Traverse (const bound_volumes::plane_listf& frustum, TraverseResult& result, size_t filter)
{
  result.Clear ();

  if (!impl->scene)
    return;

  CollectionVisitor visitor (result, filter);

  SceneCollector collector (*this, visitor);

  impl->scene->Traverse (frustum, collector);
}

void Scene::Traverse (const bound_volumes::plane_listf& frustum, TraverseResult& result)
{
  Traverse (frustum, result, Collect_All);
}
