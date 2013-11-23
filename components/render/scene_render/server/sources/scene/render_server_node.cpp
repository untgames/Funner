#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации узла
*/

struct Node::Impl
{
  stl::string name;     //имя узла
  math::mat4f world_tm; //матрица преобразований узла
  Scene*      scene;    //сцена

/// Конструкторы
  Impl ()
    : world_tm (1.0f)
    , scene ()
  {
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Node::Node ()
  : impl (new Impl)
{
}

Node::~Node ()
{
  SetSceneOwner (0);
}

/*
    Имя узла
*/

void Node::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Node::SetName", "name");

  impl->name = name;
}

const char* Node::Name () const
{
  return impl->name.c_str ();
}

/*
    Матрица преобразований
*/

void Node::SetWorldMatrix (const math::mat4f& tm)
{
  impl->world_tm = tm;
}

const math::mat4f& Node::WorldMatrix () const
{
  return impl->world_tm;
}

/*
    Сцена
*/

void Node::SetSceneOwner (Scene* scene)
{
  try
  {
    if (scene == impl->scene)
      return;

    if (impl->scene)
      impl->scene->DetachNode (this);

    impl->scene = 0;

    impl->scene->AttachNode (this);

    impl->scene = scene;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Node::SetSceneOwner");
    throw;
  }
}

Scene* Node::SceneOwner () const
{
  return impl->scene;
}

/*
    Обход
*/

void Node::Visit (ISceneVisitor& visitor)
{
  try
  {
    VisitCore (visitor);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Visit(%s='%s')", typeid (*this).name (), Name ());
    throw;
  }
}

void Node::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}
