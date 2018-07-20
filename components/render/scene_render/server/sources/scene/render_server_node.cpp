#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации узла
*/

typedef stl::list<INodeListener*> ListenerList;

struct Node::Impl
{
  stl::string   name;      //имя узла
  math::mat4f   world_tm;  //матрица преобразований узла
  ListenerList  listeners; //слушатели событий узла

/// Конструкторы
  Impl ()
    : world_tm (1.0f)
  {
  }

///Оповещение о событии
  template <class Fn> void Notify (Node& node, Fn fn)
  {
    for (ListenerList::iterator it=listeners.begin (); it!=listeners.end ();)
    {
      INodeListener& listener = **it;

      ++it;

      try
      {
        (listener.*fn)(node);
      }
      catch (...)
      {
      }
    }
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
  impl->Notify (*this, &INodeListener::OnDestroy);
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

  impl->Notify (*this, &INodeListener::OnWorldMatrixUpdated);
}

const math::mat4f& Node::WorldMatrix () const
{
  return impl->world_tm;
}

/*
    Регистрация слушателей
*/

void Node::AttachListener (INodeListener& listener)
{
  for (ListenerList::iterator it=impl->listeners.begin (), end=impl->listeners.end (); it!=end; ++it)
    if (*it == &listener)
      throw xtl::format_operation_exception ("render::scene::server::Node::AttachListener", "Listener has been already registered");

  impl->listeners.push_back (&listener);
}

void Node::DetachListener (INodeListener& listener)
{
  for (ListenerList::iterator it=impl->listeners.begin (), end=impl->listeners.end (); it!=end; ++it)
    if (*it == &listener)
    {
      impl->listeners.erase (it);
      return;
    }
}
