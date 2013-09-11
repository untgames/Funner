#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации рендера сцены
*/

typedef xtl::array<render::scene::interchange::uint32, ObjectType_Num> IdArray;

struct ClientImpl::Impl
{
  IdArray id_pool; //пул идентификаторов

/// Конструктор
  Impl ()
  {
    id_pool.assign (0);
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
    Обработчики ответов сервера
*/

void ClientImpl::Dummy ()
{
}
