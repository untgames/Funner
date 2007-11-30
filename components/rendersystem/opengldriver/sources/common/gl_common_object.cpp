#include "shared.h"

using namespace render::low_level::opengl;
using namespace render::low_level;

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
