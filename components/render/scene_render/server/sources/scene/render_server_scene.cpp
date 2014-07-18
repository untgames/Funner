#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Описание реализации сцены
*/

typedef stl::list<Entity*>                         EntityList;
typedef stl::hash_map<Node*, EntityList::iterator> EntityMap;

struct Scene::Impl: public xtl::reference_counter
{
  stl::string name;        //имя сцены
  EntityList  entity_list; //список узлов
  EntityMap   entity_map;  //карта узлов

/// Деструктор
  ~Impl ()
  {
    while (!entity_list.empty ())
      entity_list.back ()->SetSceneOwner (0);

    entity_map.clear ();
    entity_list.clear ();
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Scene::Scene ()
  : impl (new Impl)
{
}

Scene::Scene (const Scene& scene)
  : impl (scene.impl)
{
  addref (impl);
}

Scene::~Scene ()
{
  release (impl);
}

Scene& Scene::operator = (const Scene& scene)
{
  Scene tmp (scene);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    Имя сцены
*/

void Scene::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Scene::SetName", "name");

  impl->name = name;
}

const char* Scene::Name () const
{
  return impl->name.c_str ();
}

/*
    Присоединение узла
*/

void Scene::AttachNode (Entity* node)
{
  try
  {
    if (!node)
      throw xtl::make_null_argument_exception ("", "node");

    EntityMap::iterator map_iter = impl->entity_map.find (&*node);

    if (map_iter != impl->entity_map.end ())
      throw xtl::make_argument_exception ("", "node", node->Name (), "This node has been already added to scene");

    EntityList::iterator list_iter = impl->entity_list.insert (impl->entity_list.end (), &*node);

    try
    {
      impl->entity_map.insert_pair (&*node, list_iter);
    }
    catch (...)
    {
      impl->entity_list.erase (list_iter);
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Scene::AttachNode");
    throw;
  }
}

void Scene::DetachNode (Entity* node)
{
  if (!node)
    return;

  EntityMap::iterator iter = impl->entity_map.find (&*node);

  if (iter == impl->entity_map.end ())
    return;

  impl->entity_list.erase (iter->second);
  impl->entity_map.erase (iter);
}

void Scene::Traverse (const bound_volumes::plane_listf& frustum, ISceneVisitor& visitor)
{
  for (EntityList::iterator iter=impl->entity_list.begin (), end=impl->entity_list.end (); iter!=end; ++iter)
  {
    Entity& entity = **iter;

      //если объект невидим - исключаем его из обхода

    if (!entity.IsVisible ())
      continue;

      //если объект имеет бесконечные ограничивающие объёмы - обрабатываем его

    if (entity.IsInfiniteBounds ())
    {
      entity.Visit (visitor);
      continue;
    }

      //если объект имеет конечные ограничивающие объёмы - проверяем его попадание в заданный объём

    if (intersects (frustum, entity.BoundBox ()))
      entity.Visit (visitor);
  }
}

void Scene::Traverse (const bound_volumes::plane_listf& frustum, TraverseResult& result, size_t filter)
{
  result.Clear ();

  CollectionVisitor visitor (result, filter);

  Traverse (frustum, visitor);
}

void Scene::Traverse (const bound_volumes::plane_listf& frustum, TraverseResult& result)
{
  Traverse (frustum, result, Collect_All);
}
