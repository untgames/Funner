#ifndef MEDIALIB_COLLADA_SHARED_HEADER
#define MEDIALIB_COLLADA_SHARED_HEADER

#include <media/collada/model.h>
#include <common/singleton.h>
#include <xtl/functional>
#include <stl/hash_map>
#include <shared/resource_manager.h>

namespace medialib
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система управления моделями
///////////////////////////////////////////////////////////////////////////////////////////////////
class ModelSystemImpl : public ResourceManager<ModelSystem::LoadHandler, ModelSystem::SaveHandler>
{
  public:
    ModelSystemImpl ();
};

typedef common::Singleton<ModelSystemImpl> ModelSystemSingleton;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени модели
///////////////////////////////////////////////////////////////////////////////////////////////////
const char* get_model_name (ModelImpl*);

}

}

#endif
