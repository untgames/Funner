#include "shared.h"

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    Описание реализации StaticMesh
*/

struct StaticMesh::Impl: public xtl::instance_counter<StaticMesh>
{
  string mesh_name;       //имя меша  
  size_t mesh_name_hash;  //хэш имени мэша
  
///Конструктор
  Impl () : mesh_name_hash (0xffffffff) {}
};

/*
    Конструктор / деструктор
*/

StaticMesh::StaticMesh ()
  : impl (new Impl)
  {}

StaticMesh::~StaticMesh ()
{
  delete impl;
}

/*
    Создание модели
*/

StaticMesh::Pointer StaticMesh::Create ()
{
  return Pointer (new StaticMesh, false);
}

/*
    Установка имени меша модели
*/

void StaticMesh::SetMeshName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::StaticMesh::SetMeshName", "name");
    
  impl->mesh_name      = name;
  impl->mesh_name_hash = strhash (name);
}

const char* StaticMesh::MeshName () const
{
  return impl->mesh_name.c_str ();
}

size_t StaticMesh::MeshNameHash () const
{
  return impl->mesh_name_hash;
}

/*
    Метод, вызываемый при посещении объекта
*/

void StaticMesh::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    Связывание свойств
*/

void StaticMesh::BindProperties (common::PropertyBindingMap& bindings)
{
  Entity::BindProperties (bindings);

  bindings.AddProperty ("Mesh", xtl::bind (&StaticMesh::MeshName, this), xtl::bind (&StaticMesh::SetMeshName, this, _1));
}
