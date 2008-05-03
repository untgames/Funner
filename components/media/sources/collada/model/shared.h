#ifndef MEDIALIB_COLLADA_SHARED_HEADER
#define MEDIALIB_COLLADA_SHARED_HEADER

#include <stl/string>
#include <stl/vector>
#include <stl/hash_map>

#include <xtl/reference_counter.h>
#include <xtl/functional>

#include <common/exception.h>
#include <common/singleton.h>

#include <media/collada.h>

#include <shared/clone.h>
#include <shared/resource_holder.h>
#include <shared/resource_manager.h>
#include <shared/resource_library.h>

#include "library.h"
#include "collection.h"

namespace media
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация менеджера моделей
///////////////////////////////////////////////////////////////////////////////////////////////////
class ModelManagerImpl : public ResourceManager<ModelManager::LoadHandler, ModelManager::SaveHandler>
{
  public:
    ModelManagerImpl ();
};

typedef common::Singleton<ModelManagerImpl> ModelManagerSingleton;

}

}

#endif
