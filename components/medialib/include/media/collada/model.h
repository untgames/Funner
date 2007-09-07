#ifndef MEDIALIB_COLLADA_MODEL_HEADER
#define MEDIALIB_COLLADA_MODEL_HEADER

#include <xtl/functional_fwd>
#include <stl/auto_ptr.h>
#include <media/collada/scene.h>
#include <media/collada/morph.h>
#include <media/collada/skin.h>

namespace media
{

namespace collada
{

//implementation forwards
struct ModelImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef ILibrary<Effect>   EffectLibrary;
typedef ILibrary<Material> MaterialLibrary;
typedef ILibrary<Mesh>     MeshLibrary;
typedef ILibrary<Morph>    MorphLibrary;
typedef ILibrary<Skin>     SkinLibrary;
typedef ILibrary<Light>    LightLibrary;
typedef ILibrary<Camera>   CameraLibrary;
typedef ILibrary<Node>     NodeLibrary;
typedef ILibrary<Scene>    SceneLibrary;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Модель
///////////////////////////////////////////////////////////////////////////////////////////////////
class Model
{
  public:
    typedef xtl::function<void (const char*)> LogFunction;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Model  ();
    Model  (const char* file_name);
    Model  (const char* file_name, const LogFunction& log_func);
    ~Model ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя модели
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Load (const char* file_name, const LogFunction& log);
    void Save (const char* file_name);
    void Save (const char* file_name, const LogFunction& log);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя активной сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ActiveSceneName    () const;
    void        SetActiveSceneName (const char* scene_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
          collada::MaterialLibrary& Materials ();
          collada::EffectLibrary&   Effects   ();
          collada::MeshLibrary&     Meshes    ();
          collada::MorphLibrary&    Morphs    ();
          collada::SkinLibrary&     Skins     ();          
          collada::LightLibrary&    Lights    ();
          collada::CameraLibrary&   Cameras   ();
          collada::NodeLibrary&     Nodes     ();
          collada::SceneLibrary&    Scenes    ();
    const collada::MaterialLibrary& Materials () const;
    const collada::EffectLibrary&   Effects   () const;
    const collada::MeshLibrary&     Meshes    () const;
    const collada::MorphLibrary&    Morphs    () const;
    const collada::SkinLibrary&     Skins     () const;
    const collada::LightLibrary&    Lights    () const;
    const collada::CameraLibrary&   Cameras   () const;
    const collada::NodeLibrary&     Nodes     () const;
    const collada::SceneLibrary&    Scenes    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Model&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчики загрузки/сохранения по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void DefaultDaeLoader (const char* file_name, Model&, const LogFunction& log);

  private:
    Model  (const Model&); //no impl
    Model& operator = (const Model&); //no impl

  private:
    stl::auto_ptr<ModelImpl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Model&, Model&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система управления моделями
///////////////////////////////////////////////////////////////////////////////////////////////////
class ModelSystem
{
  public:
    typedef xtl::function<void (const char*,       Model&, const Model::LogFunction& log)> LoadHandler;
    typedef xtl::function<void (const char*, const Model&, const Model::LogFunction& log)> SaveHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с пользовательскими функциями загрузки и сохранения
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterLoader   (const char* extension, const LoadHandler& loader);
    static void RegisterSaver    (const char* extension, const SaveHandler& saver);
    static void UnregisterLoader (const char* extension);
    static void UnregisterSaver  (const char* extension);
    static void UnregisterAll    ();
};

}

}

#endif
