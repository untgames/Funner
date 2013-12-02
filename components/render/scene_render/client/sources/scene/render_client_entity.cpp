#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

/*
    Описание реализации сущности
*/

struct Entity::Impl
{
  size_t world_bounds_hash;
  bool   is_infinite;
  bool   is_visible;

  Impl () : world_bounds_hash (~0u), is_infinite (false), is_visible (true) {}
};

/*
    Конструктор / деструктор
*/

Entity::Entity (scene_graph::Entity& entity, SceneManager& manager, interchange::NodeType node_type)
  : Node (entity, manager, node_type)
  , impl (new Impl)
{
}

Entity::~Entity ()
{
}

/*
    Реализация синхронизации
*/

void Entity::UpdateCore (client::Context& context)
{
  try
  {
    Node::UpdateCore (context);

    scene_graph::Entity& entity = SourceNode ();

      //синхронизация видимости

    bool is_visible = entity.IsVisible ();

    if (is_visible != impl->is_visible)
    {
      context.SetEntityVisibility (Id (), is_visible);

      impl->is_visible = is_visible;
    }

      //синхронизация ограничивающего объема

    bool   is_infinite       = entity.IsInfiniteBounds ();
    size_t world_bounds_hash = entity.WorldBoundBoxHash ();

    if (is_infinite != impl->is_infinite || world_bounds_hash != impl->world_bounds_hash)
    {
      context.SetEntityBounds (Id (), is_infinite, entity.WorldBoundBox ());

      impl->is_infinite       = is_infinite;
      impl->world_bounds_hash = world_bounds_hash;
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Entity::UpdateCore");
    throw;
  }
}
