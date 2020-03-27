#include "shared.h"

using namespace media::geometry;

namespace
{

///Реализация пула идентификаторов
class IdPoolImpl
{
  public:
    object_id_t AllocateId (ObjectType type)
    {
      static const char* METHOD_NAME = "media::geometry::IdPool::AllocateId";

      if (type < 0 || type >= ObjectType_Num)
        throw xtl::make_argument_exception (METHOD_NAME, "type", type);

      object_id_t& id = id_pool [type];

      if (!(id + 1))
        throw xtl::format_operation_exception (METHOD_NAME, "ID pool is full for object type %d", type);

      return ++id;
    }

  private:
    typedef xtl::array<object_id_t, ObjectType_Num> IdArray;

  private:
    IdArray id_pool; //пул идентификаторов
};

typedef common::Singleton<IdPoolImpl> IdPoolSingleton;

}

/*
    Выделение идентификаторов
*/

object_id_t IdPool::AllocateId (ObjectType type)
{
  return IdPoolSingleton::Instance ()->AllocateId (type);
}
