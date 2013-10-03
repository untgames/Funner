#include "shared.h"

using namespace render;
using namespace render::scene::server;

/*
    RenderTargetDesc
*/

struct RenderTargetDesc::Impl: public xtl::reference_counter
{
  stl::string                 name;
  manager::RenderTarget       target;
  render::scene::server::Rect rect;

  Impl (const char* in_name, const manager::RenderTarget& in_target, const render::scene::server::Rect& in_rect) : name (in_name), target (in_target), rect (in_rect) {}
};

RenderTargetDesc::RenderTargetDesc (const char* name, const manager::RenderTarget& target, const render::scene::server::Rect& rect)
{
  if (!name)
    throw xtl::make_argument_exception ("render::scene::server::RenderTargetDesc::RenderTargetDesc", "name");

  impl = new Impl (name, target, rect);
}

RenderTargetDesc::RenderTargetDesc (const RenderTargetDesc& desc)
  : impl (desc.impl)
{
  addref (impl);
}

RenderTargetDesc::~RenderTargetDesc ()
{
  release (impl);
}

RenderTargetDesc& RenderTargetDesc::operator = (const RenderTargetDesc& desc)
{
  RenderTargetDesc tmp (desc);

  stl::swap (impl, tmp.impl);

  return *this;
}

const char* RenderTargetDesc::Name () const
{
  return impl->name.c_str ();
}

const Rect& RenderTargetDesc::Rect () const
{
  return impl->rect;
}

const manager::RenderTarget& RenderTargetDesc::Target () const
{
  return impl->target;
}

manager::RenderTarget RenderTargetDesc::Target ()
{
  return impl->target;
}

/*
    Описание реализации карты целей рендеринга
*/

typedef stl::hash_map<stl::hash_key<const char*>, RenderTargetDesc> DescMap;
typedef stl::vector<IRenderTargetMapListener*>                      ListenerArray;

struct RenderTargetMap::Impl: public xtl::reference_counter
{
  DescMap       targets;   //цели рендеринга
  ListenerArray listeners; //слушатели
};

/*
    Конструкторы / деструктор / присваивание
*/

RenderTargetMap::RenderTargetMap ()
  : impl (new Impl)
{
}

RenderTargetMap::RenderTargetMap (const RenderTargetMap& map)
  : impl (map.impl)
{
  addref (impl);
}

RenderTargetMap::~RenderTargetMap ()
{
  release (impl);
}

RenderTargetMap& RenderTargetMap::operator = (const RenderTargetMap& map)
{
  RenderTargetMap tmp (map);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    Количество целей
*/

size_t RenderTargetMap::Size () const
{
  return impl->targets.size ();
}

/*
    Поиск по имени
*/

const RenderTargetDesc* RenderTargetMap::Find (const char* name) const
{
  return const_cast<RenderTargetMap&> (*this).Find (name);
}

RenderTargetDesc* RenderTargetMap::Find (const char* name)
{
  if (!name)
    return 0;

  DescMap::iterator iter = impl->targets.find (name);

  if (iter == impl->targets.end ())
    return 0;

  return &iter->second;
}

/*
    Добавление целей 
*/

void RenderTargetMap::Add (const char* name, const manager::RenderTarget& target, const Rect& rect)
{
  static const char* METHOD_NAME = "render::scene::server::RenderTargetMap::Add";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  DescMap::iterator iter = impl->targets.find (name);

  if (iter != impl->targets.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "RenderTarget with such name has been already registered");

  RenderTargetDesc desc (name, target, rect);

  impl->targets.insert_pair (name, desc);

  for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try
    {
      (*iter)->OnRenderTargetAdded (desc);
    }
    catch (...)
    {
    }
  }
}

/*
    Удаление целей
*/

void RenderTargetMap::Remove (const char* name)
{
  if (!name)
    return;

  impl->targets.erase (name);

  for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try
    {
      (*iter)->OnRenderTargetRemoved (name);
    }
    catch (...)
    {
    }
  }
}

/*
    Очистка
*/

void RenderTargetMap::Clear ()
{
  for (DescMap::iterator iter=impl->targets.begin (), end=impl->targets.end (); iter!=end; ++iter)
  {
    const char* name = iter->second.Name ();

    for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
    {
      try
      {
        (*iter)->OnRenderTargetRemoved (name);
      }
      catch (...)
      {
      }
    }
  }

  impl->targets.clear ();
}

/*
    Добавление слушателей
*/

void RenderTargetMap::AttachListener (IRenderTargetMapListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::scene::server::RenderTargetMap::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void RenderTargetMap::DetachListener (IRenderTargetMapListener* listener)
{
  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void RenderTargetMap::DetachAllListeners ()
{
  impl->listeners.clear ();
}
