#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации рендера сцены
*/

typedef xtl::array<render::scene::interchange::object_id_t, ObjectType_Num> IdArray;

namespace
{

const size_t FENCE_WAITERS_RESERVE_SIZE = 128;

/// Хранилище менеджеров
struct ManagersHolder
{
  SceneManager          scene_manager;
  MaterialManager       material_manager;
  FontManager           font_manager;

  ManagersHolder (ClientImpl& client, Context& context)
    : scene_manager (client, context)
    , material_manager (client, context)
    , font_manager (material_manager)
  {
  }
};

//Слушатель событий синхронизации
struct FenceWaiter
{
  uint32          tag;
  IFenceListener* listener;

  FenceWaiter (uint32 in_tag, IFenceListener* in_listener)
    : tag (in_tag), listener (in_listener)
    {}
};

typedef stl::vector<FenceWaiter> FenceWaiterArray;

}

struct ClientImpl::Impl
{
  client::Context*                    context;            //контекст (получать только через вызов Context ())
  IdArray                             id_pool;            //пул идентификаторов
  interchange::PropertyMapAutoWriter  properties_writer;  //синхронизатор свойств (запись на сервер)
  interchange::PropertyMapReader      properties_reader;  //синхронизатор свойств (чтение с сервера)
  stl::auto_ptr<ManagersHolder>       managers;           //менеджеры
  FenceWaiterArray                    fence_waiters;      //слушатели событий синхронизации

/// Конструктор
  Impl ()
    : context ()
  {
    id_pool.assign (0);

    fence_waiters.reserve (FENCE_WAITERS_RESERVE_SIZE);
  }

/// Получение контекста
  client::Context& Context ()
  {
    if (!context)
      throw xtl::format_operation_exception ("render::scene::client::ClientImpl::Impl::Context", "Context is null");

    return *context;
  }
};

/*
    Конструктор / деструктор
*/

ClientImpl::ClientImpl ()
  : impl (new Impl)
{
}

ClientImpl::~ClientImpl ()
{
}

/*
    Связывание с контекстом
*/

void ClientImpl::SetContext (Context* context)
{
  try
  {
    impl->context = context;

    try
    {
      impl->managers.reset ();

      if (context)
        impl->managers.reset (new ManagersHolder (*this, *context));
    }
    catch (...)
    {
      impl->context = 0;
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::SetContext");
    throw;
  }
}

/*
    Выполнение синхронизации с сервером
*/

void ClientImpl::Synchronize ()
{
  try
  {
    impl->properties_writer.Write (impl->Context ());

    if (impl->managers)
      impl->managers->scene_manager.Update ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::Synchronize");
    throw;
  }
}

/*
    Выделение идентификаторов
*/

render::scene::interchange::object_id_t ClientImpl::AllocateId (ObjectType type)
{
  static const char* METHOD_NAME = "render::scene::client::ClientImpl::AllocateId";

  if (type < 0 || type >= ObjectType_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "type", type);

  render::scene::interchange::object_id_t& id = impl->id_pool [type];

  if (!(id + 1))
    throw xtl::format_operation_exception (METHOD_NAME, "ID pool is full for object type %d", type);

  return ++id;
}

void ClientImpl::DeallocateId (ObjectType type, render::scene::interchange::object_id_t id)
{
  if (type < 0 || type >= ObjectType_Num)
    return;
}

/*
    Синхронизация свойств
*/

PropertyMapSynchronizer ClientImpl::CreateSynchronizer (const common::PropertyMap& properties)
{
  try
  {
    return impl->properties_writer.CreateSynchronizer (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::CreateSynchronizer");
    throw;
  }
}

common::PropertyMap ClientImpl::GetServerProperties (object_id_t id)
{
  try
  {
    return impl->properties_reader.GetProperties (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::GetServerProperties");
    throw;
  }
}

/*
    Обработчики ответов сервера
*/

void ClientImpl::FenceResponse (object_id_t tag)
{
  try
  {
    for (FenceWaiterArray::iterator iter = impl->fence_waiters.begin (), end = impl->fence_waiters.end (); iter != end; ++iter)
    {
      if (iter->tag == tag)
      {
        iter->listener->OnFenceTriggered (tag);
        impl->fence_waiters.erase (iter);
        break;
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::FenceResponse");
    throw;
  }
}

void ClientImpl::RemovePropertyMap (object_id_t id)
{
  impl->properties_reader.RemoveProperties (id);
}

void ClientImpl::RemovePropertyLayout (object_id_t id)
{
  impl->properties_reader.RemoveLayout (id);
}

void ClientImpl::UpdatePropertyMap (render::scene::interchange::InputStream& stream)
{
  try
  {
    impl->properties_reader.Read (stream);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::UpdatePropertyMap");
    throw;
  }
}

/*
   Синхронизация
*/

void ClientImpl::RegisterFenceListener (object_id_t tag, IFenceListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::scene::client::ClientImpl::RegisterFenceListener", "listener");

  impl->fence_waiters.push_back (FenceWaiter (tag, listener));
}

void ClientImpl::UnregisterFenceListener (object_id_t tag, IFenceListener* listener)
{
  for (FenceWaiterArray::reverse_iterator iter = impl->fence_waiters.rbegin (), end = impl->fence_waiters.rend (); iter != end; ++iter)
  {
    if (iter->tag == tag && iter->listener == listener)
    {
      impl->fence_waiters.erase (iter.base () - 1);
      break;
    }
  }
}

/*
    Менеджеры
*/

SceneManager& ClientImpl::SceneManager ()
{
  if (!impl->context)
    throw xtl::format_operation_exception ("render::scene::client::ClientImpl::SceneManager", "Can't return SceneManger: context is null");

  return impl->managers->scene_manager;
}

MaterialManager& ClientImpl::MaterialManager ()
{
  if (!impl->context)
    throw xtl::format_operation_exception ("render::scene::client::ClientImpl::MaterialManager", "Can't return MaterialManger: context is null");

  return impl->managers->material_manager;
}

FontManager& ClientImpl::FontManager ()
{
  if (!impl->context)
    throw xtl::format_operation_exception ("render::scene::client::ClientImpl::FontManager", "Can't return FontManger: context is null");

  return impl->managers->font_manager;
}
