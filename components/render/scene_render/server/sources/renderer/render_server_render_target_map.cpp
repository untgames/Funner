#include "shared.h"

using namespace render;
using namespace render::scene::server;

/*
    Константы
*/

const size_t LISTENER_ARRAY_RESERVE_SIZE = 16; //резервируемое количество слушателей

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

const Rect& RenderTargetDesc::Area () const
{
  return impl->rect;
}

void RenderTargetDesc::SetArea (const Rect& rect)
{
  impl->rect = rect;
}

const manager::RenderTarget& RenderTargetDesc::Target () const
{
  return impl->target;
}

manager::RenderTarget& RenderTargetDesc::Target ()
{
  return impl->target;
}

void RenderTargetDesc::SetTarget (const manager::RenderTarget& target)
{
  impl->target = target;
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

/// Конструктор
  Impl ()
  {
    listeners.reserve (LISTENER_ARRAY_RESERVE_SIZE);
  }
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
  if (impl->decrement ())
  {
    Clear ();

    delete impl;
  }
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

  RenderTargetDesc& desc = impl->targets.insert_pair (name, RenderTargetDesc (name, target, rect)).first->second;

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
    Обновление целей 
*/

void RenderTargetMap::SetRenderTarget (const char* name, const manager::RenderTarget& target)
{
  static const char* METHOD_NAME = "render::scene::server::RenderTargetMap::SetRenderTarget";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  DescMap::iterator iter = impl->targets.find (name);

  if (iter == impl->targets.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "RenderTarget with such name has not been registered");

  RenderTargetDesc& desc = iter->second;

  desc.SetTarget (target);

  for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try
    {
      (*iter)->OnRenderTargetUpdated (desc);
    }
    catch (...)
    {
    }
  }
}

void RenderTargetMap::SetArea (const char* name, const Rect& rect)
{
  static const char* METHOD_NAME = "render::scene::server::RenderTargetMap::SetArea(const char*,const Rect&)";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  DescMap::iterator iter = impl->targets.find (name);

  if (iter == impl->targets.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "RenderTarget with such name has not been registered");

  RenderTargetDesc& desc = iter->second;

  desc.SetArea (rect);

  for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try
    {
      (*iter)->OnRenderTargetUpdated (desc);
    }
    catch (...)
    {
    }
  }
}

void RenderTargetMap::SetArea (const Rect& rect)
{
  for (DescMap::iterator iter=impl->targets.begin (), end=impl->targets.end (); iter!=end; ++iter)
  {  
    RenderTargetDesc& desc = iter->second;

    desc.SetArea (rect);

    for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
    {
      try
      {
        (*iter)->OnRenderTargetUpdated (desc);
      }
      catch (...)
      {
      }
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

  DescMap::iterator desc_iter = impl->targets.find (name);

  if (desc_iter == impl->targets.end ())
    return;

  for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try
    {
      (*iter)->OnRenderTargetRemoved (desc_iter->second);
    }
    catch (...)
    {
    }
  }

  impl->targets.erase (desc_iter);
}

/*
    Очистка
*/

void RenderTargetMap::Clear ()
{
  for (DescMap::iterator iter=impl->targets.begin (), end=impl->targets.end (); iter!=end; ++iter)
  {
    const RenderTargetDesc& desc = iter->second;

    for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
    {
      try
      {
        (*iter)->OnRenderTargetRemoved (desc);
      }
      catch (...)
      {
      }
    }
  }

  impl->targets.clear ();
}

/*
    Принудительное оповещение об обновлении целей рендеринга
*/

void RenderTargetMap::ForceUpdateNotify ()
{
  for (DescMap::iterator iter=impl->targets.begin (), end=impl->targets.end (); iter!=end; ++iter)
  {  
    RenderTargetDesc& desc = iter->second;

    for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
    {
      try
      {
        (*iter)->OnRenderTargetUpdated (desc);
      }
      catch (...)
      {
      }
    }
  }
}

/*
    Добавление слушателей
*/

void RenderTargetMap::AttachListener (IRenderTargetMapListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::scene::server::RenderTargetMap::AttachListener", "listener");

  impl->listeners.push_back (listener);

  for (DescMap::iterator iter=impl->targets.begin (), end=impl->targets.end (); iter!=end; ++iter)
    listener->OnRenderTargetAdded (iter->second);
}

void RenderTargetMap::DetachListener (IRenderTargetMapListener* listener)
{
  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void RenderTargetMap::DetachAllListeners ()
{
  impl->listeners.clear ();
}
