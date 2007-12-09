#include "shared.h"

using namespace render::low_level::opengl;
using namespace render::low_level;
using namespace common;

namespace
{

int current_object_id = 1;

}

/*
    Конструктор
*/

Object::Object ()
  : id (current_object_id++)
{
  if (!id)
  {
    --current_object_id;
    RaiseNotSupported ("render::low_level::opengl::Object::Object", "Too many object created. No enough identifiers");
  }
}

/*
    Подсчёт ссылок
*/

void Object::AddRef ()
{
  addref (this);
}

void Object::Release ()
{
  release (this);
}

/*
    Список свойств объекта
*/

IPropertyList* Object::GetProperties ()
{
  typedef common::Singleton<DefaultPropertyList> DefaultPropertyListSingleton;
  
  return DefaultPropertyListSingleton::InstancePtr ();
}
