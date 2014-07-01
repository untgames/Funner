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

/// Конструкторы
  Impl ()
    : world_tm (1.0f)
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

  OnWorldMatrixUpdated ();
}

const math::mat4f& Node::WorldMatrix () const
{
  return impl->world_tm;
}
