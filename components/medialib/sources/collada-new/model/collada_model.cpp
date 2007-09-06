#include <media/collada/model.h>

#include <common/exception.h>
#include <stl/string>

#include "shared.h"
#include "library.h"

using namespace medialib::collada;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    Описание реализации модели
*/

namespace medialib
{

namespace collada
{

struct ModelImpl
{
  stl::string       name;         //имя модели
  stl::string       active_scene; //имя активной сцены
  Library<Effect>   effects;      //библиотека эффектов
  Library<Material> materials;    //библиотека материалов
  Library<Mesh>     meshes;       //библиотека мешей
  Library<Morph>    morphs;       //библиотека морферов
  Library<Skin>     skins;        //библиотека скинов
  Library<Node>     nodes;        //библиотека узлов  
  Library<Scene>    scenes;       //библиотека сцен
  Library<Light>    lights;       //библиотека источников света
  Library<Camera>   cameras;      //библиотека камер
  
  ModelImpl () : effects (this), materials (this), meshes (this), skins (this), morphs (this), nodes (this), scenes (this),
                 lights (this), cameras (this) {}
};

}

}

/*
    Утилиты
*/

namespace
{

void default_collada_log (const char*)
{
}

}

/*
    Конструкторы / деструктор
*/

Model::Model  ()
  : impl (new ModelImpl) {}

Model::Model (const char* file_name)
  : impl (new ModelImpl)
{
  ModelSystemSingleton::Instance ().Load (file_name, *this, &default_collada_log);
}

Model::Model (const char* file_name, const LogFunction& log)
  : impl (new ModelImpl)
{
  ModelSystemSingleton::Instance ().Load (file_name, *this, log);
}
  
Model::~Model ()
{
}

/*
    Имя модели
*/

const char* Model::Name () const
{
  return impl->name.c_str ();
}

void Model::Rename (const char* new_name)
{
  if (!new_name)
    RaiseNullArgument ("medialib::collada::Model::Rename", "new_name");
    
  impl->name = new_name;
}

/*
    Имя активной сцены
*/

const char* Model::ActiveSceneName () const
{
  return impl->active_scene.c_str ();
}

void Model::SetActiveSceneName (const char* scene_name)
{
  if (!scene_name)
    RaiseNullArgument ("medialib::collada::Model::SetActiveSceneName", "scene_name");
    
  impl->active_scene = scene_name;
}

/*
    Библиотеки
*/

MaterialLibrary& Model::Materials ()
{
  return impl->materials;
}

const MaterialLibrary& Model::Materials () const
{
  return impl->materials;
}

EffectLibrary& Model::Effects ()
{
  return impl->effects;
}

const EffectLibrary& Model::Effects () const
{
  return impl->effects;
}

MeshLibrary& Model::Meshes ()
{
  return impl->meshes;
}

const MeshLibrary& Model::Meshes () const
{
  return impl->meshes;
}

MorphLibrary& Model::Morphs ()
{
  return impl->morphs;
}

const MorphLibrary& Model::Morphs () const
{
  return impl->morphs;
}

SkinLibrary& Model::Skins ()
{
  return impl->skins;
}

const SkinLibrary& Model::Skins () const
{
  return impl->skins;
}

LightLibrary& Model::Lights ()
{
  return impl->lights;
}

const LightLibrary& Model::Lights () const
{
  return impl->lights;
}

CameraLibrary& Model::Cameras ()
{
  return impl->cameras;
}

const CameraLibrary& Model::Cameras () const
{
  return impl->cameras;
}

NodeLibrary& Model::Nodes ()
{
  return impl->nodes;
}

const NodeLibrary& Model::Nodes () const
{
  return impl->nodes;
}

SceneLibrary& Model::Scenes ()
{
  return impl->scenes;
}

const SceneLibrary& Model::Scenes () const
{
  return impl->scenes;
}

/*
    Обмен
*/

void Model::Swap (Model& model)
{
  stl::swap (model.impl, impl);
}

namespace medialib
{

namespace collada
{

void swap (Model& a, Model& b)
{
  a.Swap (b);
}

}

}

/*
    Загрузка / сохранение
*/

void Model::Load (const char* file_name)
{
  Model (file_name).Swap (*this);
}

void Model::Load (const char* file_name, const LogFunction& log)
{
  Model (file_name, log).Swap (*this);
}

void Model::Save (const char* file_name)
{
  ModelSystemSingleton::Instance ().Save (file_name, *this, &default_collada_log);
}

void Model::Save (const char* file_name, const LogFunction& log)
{
  ModelSystemSingleton::Instance ().Save (file_name, *this, log);
}

/*
    Возвращение имени модели
*/

namespace medialib
{

namespace collada
{

const char* get_model_name (ModelImpl* impl)
{
  return impl->name.c_str ();
}

}

}
