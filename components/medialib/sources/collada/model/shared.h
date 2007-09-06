#ifndef MEDIALIB_COLLADA_SHARED_HEADER
#define MEDIALIB_COLLADA_SHARED_HEADER

#include <media/collada/model.h>
#include <common/singleton.h>
#include <xtl/functional>
#include <stl/hash_map>

namespace medialib
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система управления моделями
///////////////////////////////////////////////////////////////////////////////////////////////////
class ModelSystemImpl
{
  public:
    typedef ModelSystem::LoadFunction LoadFunction;
    typedef ModelSystem::SaveFunction SaveFunction;
    typedef Model::LogFunction        LogFunction;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ModelSystemImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация пользовательских функций загрузки / сохранения
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool RegisterLoader       (const char* extension, const LoadFunction& loader);
    bool RegisterSaver        (const char* extension, const SaveFunction& saver);
    void UnregisterLoader     (const char* extension);
    void UnregisterSaver      (const char* extension);
    void UnregisterAllLoaders ();
    void UnregisterAllSavers  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / сохранение моделей
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name, Model& model, const LogFunction& log) const;
    void Save (const char* file_name, const Model& model, const LogFunction& log) const;

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, LoadFunction> LoadFunctions;
    typedef stl::hash_map<stl::hash_key<const char*>, SaveFunction> SaveFunctions;
    
  private:
    LoadFunctions loaders;
    SaveFunctions savers;
};

typedef common::Singleton<ModelSystemImpl> ModelSystemSingleton;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени модели
///////////////////////////////////////////////////////////////////////////////////////////////////
const char* get_model_name (ModelImpl*);

}

}

#endif
