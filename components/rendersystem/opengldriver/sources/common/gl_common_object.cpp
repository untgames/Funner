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
    Список свойств объекта
*/

IPropertyList* Object::GetProperties ()
{
  typedef common::Singleton<DefaultPropertyList> DefaultPropertyListSingleton;
  
  return DefaultPropertyListSingleton::InstancePtr ();
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
    Получение trackable
*/

xtl::trackable& Object::GetTrackable ()
{
  return trackable;
}

/*
    Регистрация обработчиков события удаления объекта
*/

xtl::connection Object::RegisterDestroyHandler (xtl::trackable::slot_type& handler)
{
  return trackable.connect_tracker (handler);
}

xtl::connection Object::RegisterDestroyHandler (const xtl::trackable::function_type& handler)
{
  return trackable.connect_tracker (handler);
}

xtl::connection Object::RegisterDestroyHandler (const xtl::trackable::function_type& handler, xtl::trackable& linked_trackable)
{
  return trackable.connect_tracker (handler, linked_trackable);
}
