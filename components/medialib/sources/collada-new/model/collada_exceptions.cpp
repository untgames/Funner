#include "shared.h"

namespace medialib
{

namespace collada
{

void raise_incompatible (const char* source, Entity& entity1, Entity& entity2)
{
  common::Raise<IncompatibleException> (source, "Different owners: attempt to attach entity '%s' from model"
    " '%s' to entity '%s' from model '%s'", entity1.EntityId (), get_model_name (entity1.Owner ()), entity2.EntityId (),
    get_model_name (entity2.Owner ()));
}

void raise_incompatible (const char* source, Entity& entity, IBasicCollection& collection)
{
  common::Raise<IncompatibleException> (source, "Different owners: attempt to insert entity '%s' from model"
    " '%s' in collection '%s' from model '%s'", entity.EntityId (), get_model_name (entity.Owner ()), collection.EntityId (),
    get_model_name (collection.Owner ()));
}

void raise_incompatible (const char* source, IBasicCollection& collection, Entity& entity)
{
  raise_incompatible (source, entity, collection);
}

}

}
