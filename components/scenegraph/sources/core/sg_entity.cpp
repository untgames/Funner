#include <sg/entity.h>
#include <stl/string>
#include <xtl/signal.h>
#include <xtl/visitor.h>
#include <common/exception.h>

using namespace sg;
using namespace math;
using namespace common;

/*
    Описание реализации Entity
*/

struct Entity::Impl
{
  sg::Scene*  scene;                            //сцена, которой принадлежит объект
  stl::string name;                             //имя объекта
  size_t      name_hash;                        //хэш имени
  size_t      ref_count;                        //количество ссылок на объект
  Entity*     parent;                           //родительский узел
  Entity*     first_child;                      //первый потомок
  Entity*     last_child;                       //последний потомок
  Entity*     prev_child;                       //предыдущий потомок
  Entity*     next_child;                       //следующий потомок
  vec3f       color;                            //цвет объекта
  Signal      signals [EntityEvent_Num];        //сигналы
  bool        signal_process [EntityEvent_Num]; //флаги обработки сигналов
  vec3f       local_position;                   //локальное положение в пространстве
  quatf       local_orientation;                //локальная ориентация объекта
  vec3f       local_scale;                      //локальный масштаб объекта
  mat4f       local_tm;                         //матрица локальных преобразований
  vec3f       world_position;                   //мировое положение в пространстве
  quatf       world_orientation;                //мировая ориентация объекта
  vec3f       world_scale;                      //мировой масштаб объекта
  mat4f       world_tm;                         //матрица мировых преобразований
  bool        orientation_inherit;              //флаг наследования родительской ориентации
  bool        scale_inherit;                    //флаг наследования родительского масштаба
  bool        need_world_transform_update;      //флаг, сигнализирующий о необходимости пересчёта мировых преобразований
  bool        need_world_tm_update;             //флаг, сигнализирующий о необходимости пересчёта матрицы мировых преобразований
  bool        need_local_tm_update;             //флаг, сигнализирующий о необходимости пересчёта матрицы локальных преобразований
  size_t      update_lock;                      //счётчик открытых транзакций обновления
  bool        update_notify;                    //флаг, сигнализирующий о необходимости оповещения об обновлениях по завершении транзакции обновления
};

/*
    Конструктор / деструктор
*/

Entity::Entity ()
  : impl (new Impl)
{
  impl->scene         = 0;    //по умолчанию объект не принадлежит ни одной сцене
  impl->ref_count     = 1;
  impl->parent        = 0;
  impl->first_child   = 0;
  impl->last_child    = 0;
  impl->prev_child    = 0;
  impl->next_child    = 0;
  impl->update_lock   = 0;
  impl->update_notify = false;
  impl->name_hash     = strhash (impl->name.c_str ());

    //масштаб по умолчанию
    
  impl->local_scale = 1.0f;
  impl->world_scale = 1.0f;

    //по умолчанию объект наследует все преобразования родителя
  
  impl->orientation_inherit = true;
  impl->scale_inherit       = true;
  
    //преобразования рассчитаны

  impl->need_world_transform_update = false;
  impl->need_local_tm_update        = false;
  impl->need_world_tm_update        = false;  
}

Entity::~Entity ()
{
    //разрываем иерархические связи

  UnbindAllChildren ();
  Unbind ();
  
    //оповещение об удалении объекта
    
  Notify (EntityEvent_AfterDestroy);

    //удаляем реализацию объекта

  delete impl;
}

/*
    Создание объекта
*/

Entity* Entity::Create ()
{
  return new Entity;
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

void Entity::SetName (const char* name)
{
  if (!name)
    RaiseNullArgument ("sg::Entity::SetName", "name");

  impl->name      = name;
  impl->name_hash = strhash (name);
  
  UpdateNotify ();
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
    //защита от повторного удаления в обработчике

  if (impl->signal_process [EntityEvent_BeforeDestroy])
    return;            

  if (!--impl->ref_count)
  {
      //оповещаем клиентов об удалении объекта

    Notify (EntityEvent_BeforeDestroy);

    delete this;
  }
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
  
  UpdateNotify ();
}

void Entity::SetColor (float red, float green, float blue)
{
  SetColor (vec3f (red, green, blue));
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

void Entity::Unbind (EntityTransformSpace invariant_space)
{
  BindToParentImpl (0, EntityBindMode_Capture, invariant_space);
}

void Entity::BindToParent (Entity& parent, EntityBindMode mode, EntityTransformSpace invariant_space)
{
  BindToParentImpl (&parent, mode, invariant_space);
}

void Entity::BindToParentImpl (Entity* parent, EntityBindMode mode, EntityTransformSpace invariant_space)
{
    //защита от вызова Bind в обработчиках соответствующих событий

  if (impl->signal_process [EntityEvent_AfterBind] || impl->signal_process [EntityEvent_BeforeUnbind])
    return;

    //определяем инвариантное пространство преобразований

  switch (invariant_space)
  {
    case EntityTransformSpace_Parent:
    case EntityTransformSpace_Local:
      break;
    case EntityTransformSpace_World:
      RaiseNotImplemented ("sg::Entity::BindToParent (invariant_space=EntityTransformSpace_World)");
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::BindToParent", "invariant_space", invariant_space);
      break;
  }
  
    //проверяем корректность режима присоединения
    
  switch (mode)
  {
    case EntityBindMode_AddRef:
    case EntityBindMode_Capture:
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::BindToParent", "mode", mode);
      break;
  }
  
    //если родитель не изменяется нет необходимости в присоединии

  if (parent == impl->parent)
    return;      

    //проверка не присоединяется ли объект к своему потомку
    
  for (Entity* entity=parent; entity; entity=entity->impl->parent)
    if (entity == this)
      RaiseInvalidArgument ("sg::Entity::BindToParent", "parent", "Attempt to bind object to one of it's child");
      
    //оповещаем клиентов об отсоединении объекта от родителя
      
  Notify (EntityEvent_BeforeUnbind);
  
    //если у объект уже есть родитель производитм действия по его отсоединению
    
  if (impl->parent)
  {
      //отсоединям объект от родителя    
    
    if (impl->prev_child) impl->prev_child->impl->next_child = impl->next_child;
    else                  impl->parent->impl->first_child    = impl->next_child;
    
    if (impl->next_child) impl->next_child->impl->prev_child = impl->prev_child;
    else                  impl->parent->impl->last_child     = impl->prev_child;    
    
      //освобождаем родителя

    impl->parent->Release ();
  }

    //связываем объект с новым родителем

  impl->parent = parent;

  if (!parent)
  {
    impl->prev_child = impl->next_child = 0;
    
    if (impl->scene)
      SetScene (0);
      
    BeginUpdate ();
      
      //родительские преобразования требуют пересчёта
  
    UpdateWorldTransformNotify ();
      
      //оповещаем клиентов о присоединении объекта к новому родителю

    Notify (EntityEvent_AfterBind);
    
    EndUpdate ();

    return;
  }

    //увеличиваем число ссылок если этого требует режим присоединения

  if (mode == EntityBindMode_AddRef)
    parent->AddRef ();

    //регистрируем объект в списке потомков родителя
    
  Impl* parent_impl = parent->impl;

  impl->prev_child   = parent_impl->last_child;
  impl->next_child   = 0;
  parent->impl->last_child = this;

  if (impl->prev_child) impl->prev_child->impl->next_child = this;
  else                  parent_impl->first_child          = this;

    //обновление указателя на сцену в потомках

  if (impl->scene != parent_impl->scene)
    SetScene (parent_impl->scene);
    
  BeginUpdate ();
      
      //родительские преобразования требуют пересчёта
  
  UpdateWorldTransformNotify ();    

      //оповещаем клиентов о присоединении объекта к новому родителю

  Notify (EntityEvent_AfterBind);
  
  EndUpdate ();
}

void Entity::UnbindChild (const char* name, EntityTransformSpace invariant_space)
{
  UnbindChild (name, EntitySearchMode_OnNextSublevel, invariant_space);
}

void Entity::UnbindChild (const char* name, EntitySearchMode mode, EntityTransformSpace invariant_space)
{
  if (!name)
    RaiseNullArgument ("sg::Entity::UnbindChild", "name");

  Entity* child = FindChild (name, mode);
  
  if (!child)
    return;
    
  child->Unbind (invariant_space);
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
  
  for (Entity* entity=impl->first_child; entity; entity=entity->impl->next_child)
    entity->SetScene (scene);
}

/*
    Поиск потомка по имени
*/

Entity* Entity::FindChild (const char* name, EntitySearchMode mode) //no throw
{
  return const_cast<Entity*> (const_cast<const Entity&> (*this).FindChild (name, mode));
}

const Entity* Entity::FindChild (const char* name, EntitySearchMode mode) const //no throw
{
  if (!name)
    RaiseNullArgument ("sg::Entity::FindChild", "name");    
    
  size_t name_hash = strhash (name);
    
  switch (mode)
  {
    case EntitySearchMode_OnNextSublevel:
      for (Entity* entity=impl->first_child; entity; entity=impl->next_child)
        if (entity->impl->name_hash == name_hash && entity->impl->name == name)
          return entity;

      break;
    case EntitySearchMode_OnAllSublevels:
      for (Entity* entity=impl->first_child; entity; entity=impl->next_child)
      {
        if (entity->impl->name_hash == name_hash && name == entity->impl->name)
          return entity;

        Entity* child = entity->FindChild (name, mode);

        if (child)
          return child;
      }

      break;
    default:
      RaiseInvalidArgument ("sg::Entity::FindChild", "mode", mode);
      break;
  }

  return 0;  
}

/*
    Посещение объекта с динамической диспетчеризацией по типу (применение паттерна Visitor)
*/

void Entity::Accept (Visitor& visitor) const
{
  const_cast<Entity&> (*this).AcceptCore (visitor);
}

void Entity::AcceptCore (Visitor& visitor)
{
  visitor (*this);
}

/*
    Обход потомков
*/

void Entity::Traverse (const TraverseFunction& fn, EntityTraverseMode mode)
{
  switch (mode)
  {
    case EntityTraverseMode_BottomToTop:
      break;
    case EntityTraverseMode_TopToBottom:
      fn (*this);
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::Traverse", "mode", mode);
      break;
  }  

  for (Entity* entity=impl->first_child; entity; entity=entity->impl->next_child)
    entity->Traverse (fn, mode);
    
  if (mode == EntityTraverseMode_BottomToTop)
    fn (*this);
}

void Entity::Traverse (const ConstTraverseFunction& fn, EntityTraverseMode mode) const
{
  switch (mode)
  {
    case EntityTraverseMode_BottomToTop:
      break;
    case EntityTraverseMode_TopToBottom:
      fn (*this);
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::Traverse", "mode", mode);
      break;
  }

  for (const Entity* entity=impl->first_child; entity; entity=entity->impl->next_child)
    entity->Traverse (fn, mode);

  if (mode == EntityTraverseMode_BottomToTop)
    fn (*this);
}

void Entity::TraverseAccept (Visitor& visitor, EntityTraverseMode mode) const
{
  switch (mode)
  {
    case EntityTraverseMode_BottomToTop:
      break;
    case EntityTraverseMode_TopToBottom:
      const_cast<Entity&> (*this).AcceptCore (visitor);
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::Traverse", "mode", mode);
      break;
  }

  for (const Entity* entity=impl->first_child; entity; entity=entity->impl->next_child)
    entity->TraverseAccept (visitor, mode);

  if (mode == EntityTraverseMode_BottomToTop)
    const_cast<Entity&> (*this).AcceptCore (visitor);
}

/*
    Оповещение о необходимости пересчёта мировых преобразований / пересчёт мировых преобразований
*/

inline void Entity::UpdateWorldTransformNotify ()
{
  UpdateNotify ();

  if (impl->need_world_transform_update)
    return;

  impl->need_world_transform_update = true;
  impl->need_world_tm_update        = true;
  
    //оповещение всех потомков о необходимости пересчёта мировых преобразований
  
  for (Entity* entity=impl->first_child; entity; entity=entity->impl->next_child)
    entity->UpdateWorldTransformNotify ();
}

inline void Entity::UpdateLocalTransformNotify ()
{
  impl->need_local_tm_update = true;
  
  UpdateWorldTransformNotify ();
}

void Entity::UpdateWorldTransform () const
{
  if (impl->parent)
  {
    Entity* parent = impl->parent;
    
    const quatf& parent_orientation = parent->WorldOrientation ();
    
    if (impl->orientation_inherit) impl->world_orientation = parent_orientation * impl->local_orientation;
    else                           impl->world_orientation = impl->local_orientation;

    const vec3f& parent_scale = parent->WorldScale ();

    if (impl->scale_inherit) impl->world_scale = parent_scale * impl->local_scale;
    else                     impl->world_scale = impl->local_scale;
    
    impl->world_position = parent_orientation * (parent_scale * impl->local_position) + parent->WorldPosition ();
  }
  else
  {
    impl->world_orientation = impl->local_orientation;
    impl->world_position    = impl->local_position;
    impl->world_scale       = impl->local_scale;
  }

  impl->need_world_transform_update = false;
}

/*
    Положение объекта
*/

void Entity::SetPosition (const vec3f& position)
{
  impl->local_position = position;

  UpdateLocalTransformNotify ();
}

void Entity::SetPosition  (float x, float y, float z)
{
  SetPosition (vec3f (x, y, z));
}

void Entity::ResetPosition ()
{
  SetPosition (vec3f (0.0f));
}

const vec3f& Entity::Position () const
{
  return impl->local_position;
}

const vec3f& Entity::WorldPosition () const
{
  if (impl->need_world_transform_update)
    UpdateWorldTransform ();
    
  return impl->world_position;
}

/*
    Ориентация объекта
*/

void Entity::SetOrientation (const quatf& orientation)
{
  impl->local_orientation = orientation;
  
  UpdateLocalTransformNotify ();
}

void Entity::SetOrientation (float angle_in_degrees, float axis_x, float axis_y, float axis_z)
{
  SetOrientation (fromAxisAnglef (deg2rad (angle_in_degrees), axis_x, axis_y, axis_z));
}

void Entity::SetOrientation (float pitch_in_degrees, float yaw_in_degrees, float roll_in_degrees)
{
  SetOrientation (fromEulerAnglef (deg2rad (pitch_in_degrees), deg2rad (yaw_in_degrees), deg2rad (roll_in_degrees)));
}

void Entity::ResetOrientation ()
{
  SetOrientation (quatf ());
}

const quatf& Entity::Orientation () const
{
  return impl->local_orientation;
}

const quatf& Entity::WorldOrientation () const
{
  if (impl->need_world_transform_update)
    UpdateWorldTransform ();
    
  return impl->world_orientation;
}

/*
    Масштаб объекта
*/

void Entity::SetScale (const vec3f& scale)
{
  impl->local_scale = scale;
  
  UpdateLocalTransformNotify ();
}

void Entity::SetScale (float sx, float sy, float sz)
{
  SetScale (vec3f (sx, sy, sz));
}

void Entity::ResetScale ()
{
  SetScale (vec3f (1.0f));
}

const vec3f& Entity::Scale () const
{
  return impl->local_scale;
}

const vec3f& Entity::WorldScale () const
{
  if (impl->need_world_transform_update)
    UpdateWorldTransform ();
    
  return impl->world_scale;
}

/*
    Управление наследованием преобразований
*/

//установка флага наследования родительской ориентации
void Entity::SetOrientationInherit (bool state)
{
  if (state == impl->orientation_inherit)
    return;

  impl->orientation_inherit = state;

  UpdateWorldTransformNotify ();
}

//наследуется ли родительская ориентация
bool Entity::IsOrientationInherited () const
{
  return impl->orientation_inherit;
}

//установка флага наследования родительского масштаба
void Entity::SetScaleInherit (bool state)
{
  if (state == impl->scale_inherit)
    return;

  impl->scale_inherit = state;

  UpdateWorldTransformNotify ();
}

//наследуется ли родительский масштаб
bool Entity::IsScaleInherited () const
{
  return impl->scale_inherit;
}

/*
    Кумулятивные преобразования
*/

void Entity::Translate (const math::vec3f& offset, EntityTransformSpace space)
{
  switch (space) 
  {
    case EntityTransformSpace_Local:
      impl->local_position += impl->local_orientation * offset;
      break;
    case EntityTransformSpace_Parent:
      impl->local_position += offset;
      break;
    case EntityTransformSpace_World:
      if (impl->parent) impl->local_position += invert (impl->parent->WorldOrientation ()) * offset / impl->parent->WorldScale (); 
      else              impl->local_position += offset;

      break;
    default:
      RaiseInvalidArgument ("sg::Entity::Translate", "space", space);
      break;
  }
  
  UpdateLocalTransformNotify ();
}

void Entity::Translate (float offset_x, float offset_y, float offset_z, EntityTransformSpace space)
{
  Translate (vec3f (offset_x, offset_y, offset_z), space);
}

void Entity::Rotate (const math::quatf& q, EntityTransformSpace space)
{
  switch (space)
  {
    case EntityTransformSpace_Local:
      impl->local_orientation *= q;
      break;    
    case EntityTransformSpace_Parent:
      impl->local_orientation = q * impl->local_orientation;
      break;
    case EntityTransformSpace_World:
    {
      const quatf& world_orientation = WorldOrientation ();

      impl->local_orientation = impl->local_orientation * invert (world_orientation) * q * world_orientation;
      break;
    }      
    default:
      RaiseInvalidArgument ("sg::Entity::Rotate", "space", space);
      break;
  }

  UpdateLocalTransformNotify ();
}

void Entity::Rotate (float angle_in_degrees, float axis_x, float axis_y, float axis_z, EntityTransformSpace space)
{
  Rotate (fromAxisAnglef (deg2rad (angle_in_degrees), axis_x, axis_y, axis_z), space);
}

void Entity::Rotate (float pitch_in_degrees, float yaw_in_degrees, float roll_in_degrees, EntityTransformSpace space)
{
  Rotate (fromEulerAnglef (deg2rad (pitch_in_degrees), deg2rad (yaw_in_degrees), deg2rad (roll_in_degrees)), space);
}

void Entity::Scale (const math::vec3f& scale)
{
  impl->local_scale *= scale;  
  
  UpdateLocalTransformNotify ();
}

void Entity::Scale (float sx, float sy, float sz)
{
  Scale (vec3f (sx, sy, sz));
}

/*
    Получение матриц преобразования объекта
*/

namespace
{

//композиция преобразований
void affine_compose (const vec3f& position, const quatf& orientation, const vec3f& scale, mat4f& tm)
{
  tm = math::translate (position) * mat4f (orientation) * math::scale (scale);
}

}

const mat4f& Entity::TransformationMatrix (EntityTransformSpace space) const
{
  static mat4f identity;

  switch (space)
  {
    case EntityTransformSpace_Local:
      if (impl->need_local_tm_update)
      {
        affine_compose (impl->local_position, impl->local_orientation, impl->local_scale, impl->local_tm);

        impl->need_local_tm_update = false;
      }

      return impl->local_tm;
    case EntityTransformSpace_Parent:
      return impl->parent ? impl->parent->TransformationMatrix (EntityTransformSpace_World) : identity;
    case EntityTransformSpace_World:
      if (impl->need_world_tm_update)
      {
        affine_compose (WorldPosition (), WorldOrientation (), WorldScale (), impl->world_tm);

        impl->need_world_tm_update = false;
      }

      return impl->world_tm;
    default:
      RaiseInvalidArgument ("sg::Entity::TransformationMatrix", "space", space);
      return identity;
  } 
}

/*
    Получение матрицы преобразования объекта object в системе координат данного объекта
*/

mat4f Entity::ObjectTM (Entity& object) const
{
  return invert (WorldTM ()) * object.WorldTM ();
}

/*
    Подписка на события Entity
*/

Entity::Signal& Entity::Listeners (EntityEvent event)
{
  return const_cast<Signal&> (const_cast<const Entity&> (*this).Listeners (event));
}

const Entity::Signal& Entity::Listeners (EntityEvent event) const
{
  if (event < 0 || event >= EntityEvent_Num)
    RaiseInvalidArgument ("sg::Entity::Listeners", "event", event);
    
  return impl->signals [event];
}

/*
    Оповещение клиентов о наступлении события
*/

void Entity::Notify (EntityEvent event)
{
    //проверяем нет ли рекурсивного вызова

  if (impl->signal_process [event])
    return;
    
    //устанавливаем флаг обработки события

  impl->signal_process [event] = true;
  
    //вызываем обработчики событий

  try
  {
    impl->signals [event] (*this);
  }
  catch (...)
  {
    //все исключения клиентских обработчиков событий объекта поглощаются
  }
  
    //снимаем флаг обработки события
  
  impl->signal_process [event] = false;
}

/*
    Управление транзакциями обновления
*/

void Entity::BeginUpdate ()
{
  impl->update_lock++;
}

void Entity::EndUpdate ()
{
  if (!impl->update_lock)
    RaiseNotSupported ("sg::Entity::EndUpdate", "Attempt to call EndUpdate without previous BeginUpdate call");  
    
  if (!--impl->update_lock)
  {
      //если во время транзакции обновления объект изменялся - оповещаем клиентов об изменениях
    
    if (impl->update_notify)
    {
      Notify (EntityEvent_AfterUpdate);
      
      impl->update_notify = false;
    }
  }
}

bool Entity::IsInUpdateTransaction () const
{
  return impl->update_lock > 0;  
}

//оповещение об изменении объекта
void Entity::UpdateNotify ()
{
    //если объект не находится в транзакции обновления - оповещаем клиентов об изменениях

  if (!impl->update_lock)
  {
    Notify (EntityEvent_AfterUpdate);
    return;
  }

    //иначе устанавливаем флаг, сигнализирующий о необходимости оповещения клиентов об изменениях по завершении транзакци
    //обновления

  impl->update_notify = true;    
}
