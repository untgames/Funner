#include <sg/entity.h>
#include <stl/string>
#include <common/exception.h>

using namespace sg;
using namespace math;
using namespace common;

/*
    Описание реализации Entity
*/

struct Entity::Impl
{
  sg::Scene*  scene;                     //сцена, которой принадлежит объект
  stl::string name;                      //имя объекта
  size_t      ref_count;                 //количество ссылок на объект
  Entity*     parent;                    //родительский узел
  Entity*     first_child;               //первый потомок
  Entity*     last_child;                //последний потомок
  Entity*     prev_child;                //предыдущий потомок
  Entity*     next_child;                //следующий потомок
  vec3f       color;                     //цвет объекта
  Signal      signals [EntityEvent_Num]; //сигналы
  vec3f       local_position;            //локальное положение в пространстве
  quatf       local_orientation;         //локальная ориентация объекта
  vec3f       local_scale;               //локальный масштаб объекта
  bool        transform_inherit [EntityTransform_Num]; //флаги наследования преобразований  
};

/*
    Конструктор / деструктор
*/

Entity::Entity ()
  : impl (new Impl)
{
  impl->scene       = 0;    //по умолчанию объект не принадлежит ни одной сцене
  impl->ref_count   = 1;
  impl->parent      = 0;
  impl->first_child = 0;
  impl->last_child  = 0;
  impl->prev_child  = 0;
  impl->next_child  = 0;

    //по умолчанию объект наследует все преобразования родителя
  
  for (size_t i=0; i<EntityTransform_Num; i++)
    impl->transform_inherit [i] = true;    
}

Entity::~Entity ()
{
  UnbindAllChildren ();
  Unbind ();

  try
  {
    impl->signals [EntityEvent_Destroyed] (*this);
  }
  catch (...)
  {
    //блокируем все возможные исключения
  }

  delete impl;
}

/*
    Возвращение сцены, которой принадлежит объект
*/

Scene* Entity::Scene ()
{
  return impl->scene;
}

const Scene* Entity::Scene () const
{
  return impl->scene;
}

/*
    Имя объекта
*/

const char* Entity::Name () const
{
  return impl->name.c_str ();
}

void Entity::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("sg::Entity::Rename", "name");
    
  impl->name = name;
}

/*
    Подсчёт ссылок
*/

void Entity::AddRef ()
{
  impl->ref_count++;
}

void Entity::Release ()
{
  if (!--impl->ref_count)
    delete this;
}

size_t Entity::UseCount () const
{
  return impl->ref_count;
}

/*
    Цвет объекта
*/

void Entity::SetColor (const vec3f& color)
{
  impl->color = color;
}

const vec3f& Entity::Color () const
{
  return impl->color;
}

/*
    Связи для перемещения по данному уровню иерархии
*/

Entity* Entity::Parent ()
{
  return impl->parent;
}

const Entity* Entity::Parent () const
{
  return impl->parent;
}

Entity* Entity::FirstChild ()
{
  return impl->first_child;
}

const Entity* Entity::FirstChild () const
{
  return impl->first_child;
}

Entity* Entity::LastChild ()
{
  return impl->last_child;
}

const Entity* Entity::LastChild () const
{
  return impl->last_child;
}

Entity* Entity::PrevChild ()
{
  return impl->prev_child;
}

const Entity* Entity::PrevChild () const
{
  return impl->prev_child;
}

Entity* Entity::NextChild ()
{
  return impl->next_child;
}

const Entity* Entity::NextChild () const
{
  return impl->next_child;
}

/*
    Присоединение узла к родителю
*/

void Entity::Bind (Entity* parent, EntityBindMode mode)
{
  Bind (NeedAddRef, parent, mode);
}

void Entity::Unbind (EntityBindMode mode)
{
  Bind (0, mode);
}

void Entity::Bind (AddRefFlag flag, Entity* parent, EntityBindMode mode)
{
  if (parent == impl->parent)
    return;

  switch (mode)
  {
    case EntityBindMode_SaveLocalTM:
    case EntityBindMode_SaveWorldTM:
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::Bind", "mode", "Wrong EntityBindMode %d", mode);
      break;
  }
  
    //проверка не присоединяется ли объект к своему потомку
    
  for (Entity* entity=parent; entity; entity=entity->parent)
    if (entity == this)
      RaiseInvalidArgument ("sg::Entity::Bind", "parent", "Attempt to bind object to one of it's child");

    //отсоединям объект от родителя
    
  if (impl->parent)
  {
    if (impl->prev_child) impl->prev_child->next_child = impl->next_child;
    else                  impl->parent->first_child    = impl->next_child;
    
    if (impl->next_child) impl->next_child->prev_child = impl->prev_child;
    else                  impl->parent->last_child     = impl->prev_child;    
  }  
  
    //связываем объект с новым родителем
  
  impl->parent = parent;

  if (!parent)
  {
    impl->prev_child = impl->next_child = 0;

    if (impl->scene)
      SetScene (0);

    return;
  }

    //проверяем нужно ли увеличивать число ссылок

  if (flag)
    parent->AddRef ();

    //регистрируем объект в списке потомков родителя

  impl->prev_child   = parent->last_child;
  impl->next_child   = 0;
  parent->last_child = this;

  if (impl->prev_child) impl->prev_child->next_child = this;
  else                  parent->first_child          = this;

    //обновление указателя на сцену в потомках

  if (impl->scene != parent->scene)
    SetScene (parent->scene);
}

void Entity::UnbindAllChildren ()
{
  while (impl->last_child)
    impl->last_child->Unbind ();
}

//установка указателя на сцену для объекта и всех его потомков
void Entity::SetScene (sg::Scene* scene)
{
  impl->scene = scene;
  
  for (Entity* entity=first_child; entity; entity=entity->next_child)
    entity->SetScene (scene);
}
