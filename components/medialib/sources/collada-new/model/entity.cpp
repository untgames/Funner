#include <media/collada/utility.h>
#include <stl/string>

using namespace medialib::collada;

/*
    Реализация Entity
*/

struct Entity::Impl
{
  stl::string id;    //идентификатор объекта
  ModelImpl*  owner; //владелец
    
  Impl (ModelImpl* in_owner, const char* in_id) : owner (in_owner), id (in_id) {}
};

/*
    Конструктор и деструктор
*/

Entity::Entity (ModelImpl* owner, const char* id)
  : impl (new Impl (owner, id))
  {}
  
Entity::~Entity ()
{
  delete impl;
}

/*
    Владелец
*/

ModelImpl* Entity::Owner () const
{
  return impl->owner;
}

/*
    Идентификатор объекта
*/

const char* Entity::EntityID () const
{
  return impl->id.c_str ();
}
