#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::physics;

/*
    Описание реализации физического тела
*/

struct Body::Impl
{
  stl::string model_name; //имя физической модели
};

/*
    Конструктор / деструктор
*/

Body::Body ()
  : impl (new Impl)
{
}

Body::~Body ()
{
}

/*
    Создание тела
*/

Body::Pointer Body::Create ()
{
  return Pointer (new Body, false);
}

/*
    Установка физической модели
*/

void Body::SetModelName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::physics::Body::SetModelName", "name");
    
  impl->model_name = name;
}

const char* Body::ModelName () const
{
  return impl->model_name.c_str ();
}

/*
    Метод, вызываемый при посещении данного объекта
*/

void Body::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);  
}
