#include <sg/scene.h>
#include <stl/string>
#include <xtl/intrusive_ptr.h>
#include <common/exception.h>

#include "scene_object.h"

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    Описание реализации сцены
*/

//typedef xtl::com_ptr<ISceneManager> SceneManagerPtr;

struct Scene::Impl: public SceneSpace
{
//  SceneManagerPtr scene_manager; //менеджер сцены
  string name; //имя сцены  
  Node*  root; //корень сцены
};

/*
    Конструктор / деструктор
*/

Scene::Scene ()
  : impl (new Impl)
{
    //создание корневого узла сцены
    
  impl->root = Node::Create (*this);
}

Scene::~Scene ()
{
    //освобождение корня сцены

  impl->root->UnbindAllChildren ();
  impl->root->Release ();

    //удаление реализации
  
  delete impl;
}
    
/*
    Имя сцены
*/

void Scene::SetName (const char* name)
{
  if (!name)
    RaiseNullArgument ("scene_graph::Scene::SetName", "name");
    
  impl->name = name;
}

const char* Scene::Name () const
{
  return impl->name.c_str ();
}

/*
    Корень сцены
*/

Node& Scene::Root ()
{
  return *impl->root;
}

const Node& Scene::Root () const
{
  return *impl->root;
}

/*
    Количество объектов в сцене
*/

size_t Scene::EntitiesCount () const
{
  return impl->ObjectsCount ();
}

/*
    Регистрация объектов сцены
*/

void Scene::Attach (SceneObject& object)
{
  object.BindToSpace (impl);
}

/*
    Обход объектов, принадлежащих сцене
*/

void Scene::Traverse (const TraverseFunction& fn)
{
  SceneObject* object = impl->FirstObject ();

  for (size_t i=0, count=impl->ObjectsCount (); i<count; i++, object=object->NextObject ())
    fn (object->Entity ());
}

void Scene::Traverse (const ConstTraverseFunction& fn) const
{
  const SceneObject* object = impl->FirstObject ();

  for (size_t i=0, count=impl->ObjectsCount (); i<count; i++, object=object->NextObject ())
    fn (object->Entity ());
}

void Scene::VisitEach (Visitor& visitor) const
{
  SceneObject* object = const_cast<SceneObject*> (impl->FirstObject ());

  for (size_t i=0, count=impl->ObjectsCount (); i<count; i++, object=object->NextObject ())
    object->Entity ().Accept (visitor);
}

/*
    Node
    Биндинг узлов к сцене
*/

void Node::BindToScene (scene_graph::Scene& scene, NodeBindMode mode, NodeTransformSpace invariant_space)
{
  BindToParent (scene.Root (), mode, invariant_space);
}
