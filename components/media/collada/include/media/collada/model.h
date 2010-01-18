#ifndef MEDIALIB_COLLADA_MODEL_HEADER
#define MEDIALIB_COLLADA_MODEL_HEADER

#include <xtl/functional_fwd>
#include <stl/auto_ptr.h>
#include <media/collada/library.h>
#include <common/serializer_manager.h>

namespace media
{

namespace collada
{

//forward declarations
class Effect;
class Image;
class Material;
class Mesh;
class Morph;
class Skin;
class Light;
class Camera;
class Node;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef ILibrary<Effect>   EffectLibrary;
typedef ILibrary<Image>    ImageLibrary;
typedef ILibrary<Material> MaterialLibrary;
typedef ILibrary<Mesh>     MeshLibrary;
typedef ILibrary<Morph>    MorphLibrary;
typedef ILibrary<Skin>     SkinLibrary;
typedef ILibrary<Light>    LightLibrary;
typedef ILibrary<Camera>   CameraLibrary;
typedef ILibrary<Node>     NodeLibrary;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Модель
///////////////////////////////////////////////////////////////////////////////////////////////////
class Model
{
  public:
    typedef xtl::function<void (const char*)> LogHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Model  ();
    Model  (const char* file_name);
    Model  (const char* file_name, const LogHandler& log_func);
    Model  (const Model&);
    ~Model ();
    
    Model& operator = (const Model&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя модели
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Load (const char* file_name, const LogHandler& log);
    void Save (const char* file_name);
    void Save (const char* file_name, const LogHandler& log);

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
          collada::ImageLibrary&    Images    ();
          collada::MeshLibrary&     Meshes    ();
          collada::MorphLibrary&    Morphs    ();
          collada::SkinLibrary&     Skins     ();          
          collada::LightLibrary&    Lights    ();
          collada::CameraLibrary&   Cameras   ();
          collada::NodeLibrary&     Nodes     ();
          collada::NodeLibrary&     Scenes    ();
    const collada::MaterialLibrary& Materials () const;
    const collada::EffectLibrary&   Effects   () const;
    const collada::ImageLibrary&    Images    () const;
    const collada::MeshLibrary&     Meshes    () const;
    const collada::MorphLibrary&    Morphs    () const;
    const collada::SkinLibrary&     Skins     () const;
    const collada::LightLibrary&    Lights    () const;
    const collada::CameraLibrary&   Cameras   () const;
    const collada::NodeLibrary&     Nodes     () const;
    const collada::NodeLibrary&     Scenes    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Model&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Model&, Model&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер моделей
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<void (const char* file_name, Model& model, const Model::LogHandler& log),
  void (const char* file_name, const Model& model, const Model::LogHandler& log)> ModelManager;

}

}

#endif
