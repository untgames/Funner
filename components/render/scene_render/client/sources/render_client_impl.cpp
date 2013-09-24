#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации рендера сцены
*/

typedef xtl::array<render::scene::interchange::uint32, ObjectType_Num> IdArray;

struct ClientImpl::Impl
{
  client::Context*                   context;            //контекст (получать только через вызов Context ())
  IdArray                            id_pool;            //пул идентификаторов
  interchange::PropertyMapAutoWriter properties_writer;  //синхронизатор свойств (запись на сервер)
  interchange::PropertyMapReader     properties_reader;  //синхронизатор свойств (чтение с сервера)

/// Конструктор
  Impl ()
    : context ()
  {
    id_pool.assign (0);
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
  impl->context = context;
}

/*
    Выполнение синхронизации с сервером
*/

void ClientImpl::Sync ()
{
  try
  {
    impl->properties_writer.Write (impl->Context ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::Sync");
    throw;
  }
}

/*
    Выделение идентификаторов
*/

render::scene::interchange::uint32 ClientImpl::AllocateId (ObjectType type)
{
  static const char* METHOD_NAME = "render::scene::client::ClientImpl::AllocateId";

  if (type < 0 || type >= ObjectType_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "type", type);

  render::scene::interchange::uint32& id = impl->id_pool [type];

  if (!(id + 1))
    throw xtl::format_operation_exception (METHOD_NAME, "ID pool is full for object type %d", type);

  return id++;
}

void ClientImpl::DeallocateId (ObjectType type, render::scene::interchange::uint32 id)
{
  if (type < 0 || type >= ObjectType_Num)
    return;
}

/*
    Синхронизация свойств
*/

void ClientImpl::StartPropertyMapSynchronization (const common::PropertyMap& properties)
{
  try
  {
    impl->properties_writer.Attach (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::StartPropertyMapSynchronization");
    throw;
  }
}

void ClientImpl::StopPropertyMapSynchronization (const common::PropertyMap& properties)
{
  try
  {
    impl->properties_writer.Detach (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::StopPropertyMapSynchronization");
    throw;
  }
}

common::PropertyMap ClientImpl::GetServerProperties (uint64 id)
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

void ClientImpl::RemovePropertyMap (uint64 id)
{
  impl->properties_reader.RemoveProperties (id);
}

void ClientImpl::RemovePropertyLayout (uint64 id)
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
