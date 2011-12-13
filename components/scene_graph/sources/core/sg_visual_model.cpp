#include "shared.h"

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    Описание реализации VisualModel
*/

struct VisualModel::Impl
{
  string mesh_name;       //имя меша  
  size_t mesh_name_hash;  //хэш имени мэша
  
///Конструктор
  Impl () : mesh_name_hash (0xffffffff) {}
};

/*
    Конструктор / деструктор
*/

VisualModel::VisualModel ()
  : impl (new Impl)
  {}

VisualModel::~VisualModel ()
{
  delete impl;
}

/*
    Создание модели
*/

VisualModel::Pointer VisualModel::Create ()
{
  return Pointer (new VisualModel, false);
}

/*
    Установка имени меша модели
*/

void VisualModel::SetMeshName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::VisualModel::SetMeshName", "name");
    
  impl->mesh_name      = name;
  impl->mesh_name_hash = strhash (name);
}

const char* VisualModel::MeshName () const
{
  return impl->mesh_name.c_str ();
}

size_t VisualModel::MeshNameHash () const
{
  return impl->mesh_name_hash;
}

/*
    Метод, вызываемый при посещении объекта
*/

void VisualModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
