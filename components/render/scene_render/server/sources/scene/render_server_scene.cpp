#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Описание реализации сцены
*/

typedef stl::list<NodePtr>                       NodeList;
typedef stl::hash_map<Node*, NodeList::iterator> NodeMap;

struct Scene::Impl: public xtl::reference_counter
{
  stl::string name;      //имя сцены
  NodeList    node_list; //список узлов
  NodeMap     node_map;  //карта узлов
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

void Scene::AttachNode (const NodePtr& node)
{
  try
  {
    if (!node)
      throw xtl::make_null_argument_exception ("", "node");

    NodeMap::iterator map_iter = impl->node_map.find (&*node);

    if (map_iter != impl->node_map.end ())
      throw xtl::make_argument_exception ("", "node", node->Name (), "This node has been already added to scene");

    NodeList::iterator list_iter = impl->node_list.insert (impl->node_list.end (), node);
    
    try
    {
      impl->node_map.insert_pair (&*node, list_iter);
    }
    catch (...)
    {
      impl->node_list.erase (list_iter);
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Scene::AttachNode");
    throw;
  }
}

void Scene::DetachNode (const NodePtr& node)
{
  if (!node)
    return;

  NodeMap::iterator iter = impl->node_map.find (&*node);

  if (iter == impl->node_map.end ())
    return;

  impl->node_list.erase (iter->second);
  impl->node_map.erase (iter);  
}
