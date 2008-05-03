#include "shared.h"

using namespace media::collada;
using namespace common;

/*
    Описание реализации модели
*/

struct Model::Impl
{
  stl::string           name;         //имя модели
  stl::string           active_scene; //имя активной сцены
  LibraryImpl<Effect>   effects;      //библиотека эффектов
  LibraryImpl<Material> materials;    //библиотека материалов
  LibraryImpl<Mesh>     meshes;       //библиотека мешей
  LibraryImpl<Morph>    morphs;       //библиотека морферов
  LibraryImpl<Skin>     skins;        //библиотека скинов
  LibraryImpl<Node>     nodes;        //библиотека узлов  
  LibraryImpl<Node>     scenes;       //библиотека сцен
  LibraryImpl<Light>    lights;       //библиотека источников света
  LibraryImpl<Camera>   cameras;      //библиотека камер
};

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
    Конструкторы / деструктор / присваивание
*/

Model::Model  ()
  : impl (new Impl) {}

Model::Model (const char* file_name)
  : impl (new Impl)
{
  ModelManagerSingleton::Instance ().Load (file_name, *this, LogHandler (&default_collada_log));
}

Model::Model (const char* file_name, const LogHandler& log)
  : impl (new Impl)
{
  ModelManagerSingleton::Instance ().Load (file_name, *this, log);
}

Model::Model (const Model& model)
  : impl (new Impl (*model.impl))
  {}

Model::~Model ()
{
}

Model& Model::operator = (const Model& model)
{
  Model (model).Swap (*this);
  
  return *this;
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
    RaiseNullArgument ("media::collada::Model::Rename", "new_name");
    
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
    RaiseNullArgument ("media::collada::Model::SetActiveSceneName", "scene_name");
    
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

NodeLibrary& Model::Scenes ()
{
  return impl->scenes;
}

const NodeLibrary& Model::Scenes () const
{
  return impl->scenes;
}

/*
    Обмен
*/

void Model::Swap (Model& model)
{
  swap (model.impl, impl);
}

namespace media
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

void Model::Load (const char* file_name, const LogHandler& log)
{
  Model (file_name, log).Swap (*this);
}

void Model::Save (const char* file_name)
{
  ModelManagerSingleton::Instance ().Save (file_name, *this, LogHandler (&default_collada_log));
}

void Model::Save (const char* file_name, const LogHandler& log)
{
  ModelManagerSingleton::Instance ().Save (file_name, *this, log);
}
