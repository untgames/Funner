#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации сцены
*/

struct Scene::Impl
{
  object_id_t         id;         //идентификатор сцены
  scene_graph::Scene& scene;      //сцена
  Connection&         connection; //соединение

/// Конструктор
  Impl (scene_graph::Scene& in_scene, Connection& in_connection, object_id_t in_id)
    : id (in_id)
    , scene (in_scene)
    , connection (in_connection)
  {
  }
};

/*
    Конструктор / деструктор
*/

Scene::Scene (scene_graph::Scene& scene, Connection& connection, object_id_t id)
{
  try
  {
    impl.reset (new Impl (scene, connection, id));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Scene::Scene");
    throw;
  }
}

Scene::~Scene ()
{
}

/*
    Идентификатор сцены
*/

object_id_t Scene::Id ()
{
  return impl->id;
}

/*
    Сцена-источник
*/

scene_graph::Scene& Scene::SourceScene ()
{
  return impl->scene;
}

/*
    Синхронизация
*/

void Scene::Update ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Scene::Update");
    throw;
  }
}
