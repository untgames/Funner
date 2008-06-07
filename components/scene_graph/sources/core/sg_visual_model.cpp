#include <sg/visual_model.h>
#include <stl/string>
#include <xtl/visitor.h>
#include <common/exception.h>

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    Описание реализации VisualModel
*/

struct VisualModel::Impl
{
  string mesh_name; //имя меша  
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
    raise_null_argument ("scene_graph::VisualModel::SetMeshName", "name");
    
  impl->mesh_name = name;
}

const char* VisualModel::MeshName () const
{
  return impl->mesh_name.c_str ();
}

/*
    Метод, вызываемый при посещении объекта
*/

void VisualModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
