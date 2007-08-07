#include <sg/node.h>
#include <stl/string>
#include <xtl/signal.h>
#include <xtl/visitor.h>
#include <common/exception.h>

using namespace scene_graph;
using namespace math;
using namespace common;

/*
    Описание реализации Node
*/

struct Node::Impl
{
  scene_graph::Scene* scene;                            //сцена, которой принадлежит узел
  stl::string         name;                             //имя узла
  size_t              name_hash;                        //хэш имени
  size_t              ref_count;                        //количество ссылок на узел
  Node*               parent;                           //родительский узел
  Node*               first_child;                      //первый потомок
  Node*               last_child;                       //последний потомок
  Node*               prev_child;                       //предыдущий потомок
  Node*               next_child;                       //следующий потомок
  Signal              signals [NodeEvent_Num];          //сигналы
  bool                signal_process [NodeEvent_Num];   //флаги обработки сигналов
  vec3f               local_position;                   //локальное положение
  quatf               local_orientation;                //локальная ориентация
  vec3f               local_scale;                      //локальный масштаб
  mat4f               local_tm;                         //матрица локальных преобразований
  vec3f               world_position;                   //мировое положение
  quatf               world_orientation;                //мировая ориентация
  vec3f               world_scale;                      //мировой масштаб
  mat4f               world_tm;                         //матрица мировых преобразований
  bool                orientation_inherit;              //флаг наследования родительской ориентации
  bool                scale_inherit;                    //флаг наследования родительского масштаба
  bool                need_world_transform_update;      //флаг, сигнализирующий о необходимости пересчёта мировых преобразований
  bool                need_world_tm_update;             //флаг, сигнализирующий о необходимости пересчёта матрицы мировых преобразований
  bool                need_local_tm_update;             //флаг, сигнализирующий о необходимости пересчёта матрицы локальных преобразований
  size_t              update_lock;                      //счётчик открытых транзакций обновления
  bool                update_notify;                    //флаг, сигнализирующий о необходимости оповещения об обновлениях по завершении транзакции обновления
};

/*
    Конструктор / деструктор
*/

Node::Node ()
  : impl (new Impl)
{
  impl->scene         = 0;    //по умолчанию узел не принадлежит ни одной сцене
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

    //по умолчанию узел наследует все преобразования родителя
  
  impl->orientation_inherit = true;
  impl->scale_inherit       = true;
  
    //преобразования рассчитаны

  impl->need_world_transform_update = false;
  impl->need_local_tm_update        = false;
  impl->need_world_tm_update        = false;  
}

Node::~Node ()
{
    //разрываем иерархические связи

  UnbindAllChildren ();
  Unbind ();
  
    //оповещение об удалении узла
    
  Notify (NodeEvent_AfterDestroy);

    //удаляем реализацию узла

  delete impl;
}

/*
    Создание узла
*/

Node* Node::Create ()
{
  return new Node;
}

/*
    Возвращение сцены, которой принадлежит узел
*/

Scene* Node::Scene ()
{
  return impl->scene;
}

const Scene* Node::Scene () const
{
  return impl->scene;
}

/*
    Имя узла
*/

const char* Node::Name () const
{
  return impl->name.c_str ();
}

void Node::SetName (const char* name)
{
  if (!name)
    RaiseNullArgument ("scene_graph::Node::SetName", "name");

  impl->name      = name;
  impl->name_hash = strhash (name);
  
  UpdateNotify ();
}

/*
    Подсчёт ссылок
*/

void Node::AddRef ()
{
  impl->ref_count++;
}

void Node::Release ()
{
    //защита от повторного удаления в обработчике

  if (impl->signal_process [NodeEvent_BeforeDestroy])
    return;            

  if (!--impl->ref_count)
  {
      //оповещаем клиентов об удалении узла

    Notify (NodeEvent_BeforeDestroy);

    delete this;
  }
}

size_t Node::UseCount () const
{
  return impl->ref_count;
}

/*
    Связи для перемещения по данному уровню иерархии
*/

Node* Node::Parent ()
{
  return impl->parent;
}

const Node* Node::Parent () const
{
  return impl->parent;
}

Node* Node::FirstChild ()
{
  return impl->first_child;
}

const Node* Node::FirstChild () const
{
  return impl->first_child;
}

Node* Node::LastChild ()
{
  return impl->last_child;
}

const Node* Node::LastChild () const
{
  return impl->last_child;
}

Node* Node::PrevChild ()
{
  return impl->prev_child;
}

const Node* Node::PrevChild () const
{
  return impl->prev_child;
}

Node* Node::NextChild ()
{
  return impl->next_child;
}

const Node* Node::NextChild () const
{
  return impl->next_child;
}

/*
    Присоединение узла к родителю
*/

void Node::Unbind (NodeTransformSpace invariant_space)
{
  BindToParentImpl (0, NodeBindMode_Capture, invariant_space);
}

void Node::BindToParent (Node& parent, NodeBindMode mode, NodeTransformSpace invariant_space)
{
  BindToParentImpl (&parent, mode, invariant_space);
}

void Node::BindToParentImpl (Node* parent, NodeBindMode mode, NodeTransformSpace invariant_space)
{
    //защита от вызова Bind в обработчиках соответствующих событий

  if (impl->signal_process [NodeEvent_AfterBind] || impl->signal_process [NodeEvent_BeforeUnbind])
    return;

    //определяем инвариантное пространство преобразований

  switch (invariant_space)
  {
    case NodeTransformSpace_Parent:
    case NodeTransformSpace_Local:
      break;
    case NodeTransformSpace_World:
      RaiseNotImplemented ("scene_graph::Node::BindToParent (invariant_space=NodeTransformSpace_World)");
      break;
    default:
      RaiseInvalidArgument ("scene_graph::Node::BindToParent", "invariant_space", invariant_space);
      break;
  }
  
    //проверяем корректность режима присоединения
    
  switch (mode)
  {
    case NodeBindMode_AddRef:
    case NodeBindMode_Capture:
      break;
    default:
      RaiseInvalidArgument ("scene_graph::Node::BindToParent", "mode", mode);
      break;
  }
  
    //если родитель не изменяется нет необходимости в присоединии

  if (parent == impl->parent)
    return;      

    //проверка не присоединяется ли узел к своему потомку
    
  for (Node* Node=parent; Node; Node=Node->impl->parent)
    if (Node == this)
      RaiseInvalidArgument ("scene_graph::Node::BindToParent", "parent", "Attempt to bind object to one of it's child");
      
    //оповещаем клиентов об отсоединении узла от родителя
      
  Notify (NodeEvent_BeforeUnbind);
  
    //если у узла уже есть родитель производитм действия по его отсоединению
    
  if (impl->parent)
  {
      //отсоединям узел от родителя    
    
    if (impl->prev_child) impl->prev_child->impl->next_child = impl->next_child;
    else                  impl->parent->impl->first_child    = impl->next_child;
    
    if (impl->next_child) impl->next_child->impl->prev_child = impl->prev_child;
    else                  impl->parent->impl->last_child     = impl->prev_child;    
    
      //освобождаем родителя

    impl->parent->Release ();
  }

    //связываем узел с новым родителем

  impl->parent = parent;

  if (!parent)
  {
    impl->prev_child = impl->next_child = 0;
    
    if (impl->scene)
      SetScene (0);
      
    BeginUpdate ();
      
      //родительские преобразования требуют пересчёта
  
    UpdateWorldTransformNotify ();
      
      //оповещаем клиентов о присоединении узла к новому родителю

    Notify (NodeEvent_AfterBind);
    
    EndUpdate ();

    return;
  }

    //увеличиваем число ссылок если этого требует режим присоединения

  if (mode == NodeBindMode_AddRef)
    parent->AddRef ();

    //регистрируем узел в списке потомков родителя
    
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

      //оповещаем клиентов о присоединении узла к новому родителю

  Notify (NodeEvent_AfterBind);
  
  EndUpdate ();
}

void Node::UnbindChild (const char* name, NodeTransformSpace invariant_space)
{
  UnbindChild (name, NodeSearchMode_OnNextSublevel, invariant_space);
}

void Node::UnbindChild (const char* name, NodeSearchMode mode, NodeTransformSpace invariant_space)
{
  if (!name)
    RaiseNullArgument ("scene_graph::Node::UnbindChild", "name");

  Node* child = FindChild (name, mode);
  
  if (!child)
    return;
    
  child->Unbind (invariant_space);
}

void Node::UnbindAllChildren ()
{
  while (impl->last_child)
    impl->last_child->Unbind ();
}

//установка указателя на сцену для узла и всех его потомков
void Node::SetScene (scene_graph::Scene* scene)
{
  impl->scene = scene;
  
  for (Node* Node=impl->first_child; Node; Node=Node->impl->next_child)
    Node->SetScene (scene);
}

/*
    Поиск потомка по имени
*/

Node* Node::FindChild (const char* name, NodeSearchMode mode) //no throw
{
  return const_cast<Node*> (const_cast<const Node&> (*this).FindChild (name, mode));
}

const Node* Node::FindChild (const char* name, NodeSearchMode mode) const //no throw
{
  if (!name)
    RaiseNullArgument ("scene_graph::Node::FindChild", "name");    
    
  size_t name_hash = strhash (name);
    
  switch (mode)
  {
    case NodeSearchMode_OnNextSublevel:
      for (const Node* node=impl->first_child; node; node=impl->next_child)
        if (node->impl->name_hash == name_hash && node->impl->name == name)
          return node;

      break;
    case NodeSearchMode_OnAllSublevels:
      for (const Node* node=impl->first_child; node; node=impl->next_child)
      {
        if (node->impl->name_hash == name_hash && name == node->impl->name)
          return node;

        const Node* child = node->FindChild (name, mode);

        if (child)
          return child;
      }

      break;
    default:
      RaiseInvalidArgument ("scene_graph::Node::FindChild", "mode", mode);
      break;
  }

  return 0;  
}

/*
    Посещение узла с динамической диспетчеризацией по типу (применение паттерна Visitor)
*/

void Node::Accept (Visitor& visitor) const
{
  const_cast<Node&> (*this).AcceptCore (visitor);
}

void Node::AcceptCore (Visitor& visitor)
{
  visitor (*this);
}

/*
    Обход потомков
*/

void Node::Traverse (const TraverseFunction& fn, NodeTraverseMode mode)
{
  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_TopToBottom:
      fn (*this);
      break;
    default:
      RaiseInvalidArgument ("scene_graph::Node::Traverse", "mode", mode);
      break;
  }  

  for (Node* node=impl->first_child; node; node=node->impl->next_child)
    node->Traverse (fn, mode);
    
  if (mode == NodeTraverseMode_BottomToTop)
    fn (*this);
}

void Node::Traverse (const ConstTraverseFunction& fn, NodeTraverseMode mode) const
{
  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_TopToBottom:
      fn (*this);
      break;
    default:
      RaiseInvalidArgument ("scene_graph::Node::Traverse", "mode", mode);
      break;
  }

  for (const Node* node=impl->first_child; node; node=node->impl->next_child)
    node->Traverse (fn, mode);

  if (mode == NodeTraverseMode_BottomToTop)
    fn (*this);
}

void Node::TraverseAccept (Visitor& visitor, NodeTraverseMode mode) const
{
  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_TopToBottom:
      const_cast<Node&> (*this).AcceptCore (visitor);
      break;
    default:
      RaiseInvalidArgument ("scene_graph::Node::Traverse", "mode", mode);
      break;
  }

  for (const Node* node=impl->first_child; node; node=node->impl->next_child)
    node->TraverseAccept (visitor, mode);

  if (mode == NodeTraverseMode_BottomToTop)
    const_cast<Node&> (*this).AcceptCore (visitor);
}

/*
    Оповещение о необходимости пересчёта мировых преобразований / пересчёт мировых преобразований
*/

inline void Node::UpdateWorldTransformNotify ()
{
  UpdateNotify ();

  if (impl->need_world_transform_update)
    return;      

  impl->need_world_transform_update = true;
  impl->need_world_tm_update        = true;
  
    //оповещение производных классов об изменении положения объекта

  try
  {
    UpdateWorldTransformEvent ();
  }
  catch (...)
  {
    //поглощаем все исключения
  }

    //оповещение всех потомков о необходимости пересчёта мировых преобразований
  
  for (Node* node=impl->first_child; node; node=node->impl->next_child)
    node->UpdateWorldTransformNotify ();
}

inline void Node::UpdateLocalTransformNotify ()
{
  impl->need_local_tm_update = true;
  
  UpdateWorldTransformNotify ();
}

void Node::UpdateWorldTransform () const
{
  if (impl->parent)
  {
    Node* parent = impl->parent;
    
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
    Положение узла
*/

void Node::SetPosition (const vec3f& position)
{
  impl->local_position = position;

  UpdateLocalTransformNotify ();
}

void Node::SetPosition  (float x, float y, float z)
{
  SetPosition (vec3f (x, y, z));
}

void Node::ResetPosition ()
{
  SetPosition (vec3f (0.0f));
}

const vec3f& Node::Position () const
{
  return impl->local_position;
}

const vec3f& Node::WorldPosition () const
{
  if (impl->need_world_transform_update)
    UpdateWorldTransform ();
    
  return impl->world_position;
}

/*
    Ориентация узла
*/

void Node::SetOrientation (const quatf& orientation)
{
  impl->local_orientation = orientation;
  
  UpdateLocalTransformNotify ();
}

void Node::SetOrientation (float angle_in_degrees, float axis_x, float axis_y, float axis_z)
{
  SetOrientation (fromAxisAnglef (deg2rad (angle_in_degrees), axis_x, axis_y, axis_z));
}

void Node::SetOrientation (float pitch_in_degrees, float yaw_in_degrees, float roll_in_degrees)
{
  SetOrientation (fromEulerAnglef (deg2rad (pitch_in_degrees), deg2rad (yaw_in_degrees), deg2rad (roll_in_degrees)));
}

void Node::ResetOrientation ()
{
  SetOrientation (quatf ());
}

const quatf& Node::Orientation () const
{
  return impl->local_orientation;
}

const quatf& Node::WorldOrientation () const
{
  if (impl->need_world_transform_update)
    UpdateWorldTransform ();
    
  return impl->world_orientation;
}

/*
    Масштаб узла
*/

void Node::SetScale (const vec3f& scale)
{
  impl->local_scale = scale;
  
  UpdateLocalTransformNotify ();
}

void Node::SetScale (float sx, float sy, float sz)
{
  SetScale (vec3f (sx, sy, sz));
}

void Node::ResetScale ()
{
  SetScale (vec3f (1.0f));
}

const vec3f& Node::Scale () const
{
  return impl->local_scale;
}

const vec3f& Node::WorldScale () const
{
  if (impl->need_world_transform_update)
    UpdateWorldTransform ();
    
  return impl->world_scale;
}

/*
    Управление наследованием преобразований
*/

//установка флага наследования родительской ориентации
void Node::SetOrientationInherit (bool state)
{
  if (state == impl->orientation_inherit)
    return;

  impl->orientation_inherit = state;

  UpdateWorldTransformNotify ();
}

//наследуется ли родительская ориентация
bool Node::IsOrientationInherited () const
{
  return impl->orientation_inherit;
}

//установка флага наследования родительского масштаба
void Node::SetScaleInherit (bool state)
{
  if (state == impl->scale_inherit)
    return;

  impl->scale_inherit = state;

  UpdateWorldTransformNotify ();
}

//наследуется ли родительский масштаб
bool Node::IsScaleInherited () const
{
  return impl->scale_inherit;
}

/*
    Кумулятивные преобразования
*/

void Node::Translate (const math::vec3f& offset, NodeTransformSpace space)
{
  switch (space) 
  {
    case NodeTransformSpace_Local:
      impl->local_position += impl->local_orientation * offset;
      break;
    case NodeTransformSpace_Parent:
      impl->local_position += offset;
      break;
    case NodeTransformSpace_World:
      if (impl->parent) impl->local_position += invert (impl->parent->WorldOrientation ()) * offset / impl->parent->WorldScale (); 
      else              impl->local_position += offset;

      break;
    default:
      RaiseInvalidArgument ("scene_graph::Node::Translate", "space", space);
      break;
  }
  
  UpdateLocalTransformNotify ();
}

void Node::Translate (float offset_x, float offset_y, float offset_z, NodeTransformSpace space)
{
  Translate (vec3f (offset_x, offset_y, offset_z), space);
}

void Node::Rotate (const math::quatf& q, NodeTransformSpace space)
{
  switch (space)
  {
    case NodeTransformSpace_Local:
      impl->local_orientation *= q;
      break;    
    case NodeTransformSpace_Parent:
      impl->local_orientation = q * impl->local_orientation;
      break;
    case NodeTransformSpace_World:
    {
      const quatf& world_orientation = WorldOrientation ();

      impl->local_orientation = impl->local_orientation * invert (world_orientation) * q * world_orientation;
      break;
    }      
    default:
      RaiseInvalidArgument ("scene_graph::Node::Rotate", "space", space);
      break;
  }

  UpdateLocalTransformNotify ();
}

void Node::Rotate (float angle_in_degrees, float axis_x, float axis_y, float axis_z, NodeTransformSpace space)
{
  Rotate (fromAxisAnglef (deg2rad (angle_in_degrees), axis_x, axis_y, axis_z), space);
}

void Node::Rotate (float pitch_in_degrees, float yaw_in_degrees, float roll_in_degrees, NodeTransformSpace space)
{
  Rotate (fromEulerAnglef (deg2rad (pitch_in_degrees), deg2rad (yaw_in_degrees), deg2rad (roll_in_degrees)), space);
}

void Node::Scale (const math::vec3f& scale)
{
  impl->local_scale *= scale;  
  
  UpdateLocalTransformNotify ();
}

void Node::Scale (float sx, float sy, float sz)
{
  Scale (vec3f (sx, sy, sz));
}

/*
    Получение матриц преобразования узла
*/

namespace
{

//композиция преобразований
void affine_compose (const vec3f& position, const quatf& orientation, const vec3f& scale, mat4f& tm)
{
  tm = math::translate (position) * mat4f (orientation) * math::scale (scale);
}

}

const mat4f& Node::TransformationMatrix (NodeTransformSpace space) const
{
  static mat4f idNode;

  switch (space)
  {
    case NodeTransformSpace_Local:
      if (impl->need_local_tm_update)
      {
        affine_compose (impl->local_position, impl->local_orientation, impl->local_scale, impl->local_tm);

        impl->need_local_tm_update = false;
      }

      return impl->local_tm;
    case NodeTransformSpace_Parent:
      return impl->parent ? impl->parent->TransformationMatrix (NodeTransformSpace_World) : idNode;
    case NodeTransformSpace_World:
      if (impl->need_world_tm_update)
      {
        affine_compose (WorldPosition (), WorldOrientation (), WorldScale (), impl->world_tm);

        impl->need_world_tm_update = false;
      }

      return impl->world_tm;
    default:
      RaiseInvalidArgument ("scene_graph::Node::TransformationMatrix", "space", space);
      return idNode;
  } 
}

/*
    Получение матрицы преобразования узла object в системе координат данного узла
*/

mat4f Node::ObjectTM (Node& object) const
{
  return invert (WorldTM ()) * object.WorldTM ();
}

/*
    Подписка на события Node
*/

Node::Signal& Node::Event (NodeEvent event)
{
  return const_cast<Signal&> (const_cast<const Node&> (*this).Event (event));
}

const Node::Signal& Node::Event (NodeEvent event) const
{
  if (event < 0 || event >= NodeEvent_Num)
    RaiseInvalidArgument ("scene_graph::Node::Event", "event", event);
    
  return impl->signals [event];
}

/*
    Оповещение клиентов о наступлении события
*/

void Node::Notify (NodeEvent event)
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
    //все исключения клиентских обработчиков событий узла поглощаются
  }
  
    //снимаем флаг обработки события
  
  impl->signal_process [event] = false;
}

/*
    Управление транзакциями обновления
*/

void Node::BeginUpdate ()
{
  impl->update_lock++;
}

void Node::EndUpdate ()
{
  if (!impl->update_lock)
    RaiseNotSupported ("scene_graph::Node::EndUpdate", "Attempt to call EndUpdate without previous BeginUpdate call");  
    
  if (!--impl->update_lock)
  {
      //если во время транзакции обновления узел изменялся - оповещаем клиентов об изменениях
    
    if (impl->update_notify)
    {
      Notify (NodeEvent_AfterUpdate);
      
      impl->update_notify = false;
    }
  }
}

bool Node::IsInUpdateTransaction () const
{
  return impl->update_lock > 0;  
}

//оповещение об изменении узла
void Node::UpdateNotify ()
{
    //если узел не находится в транзакции обновления - оповещаем клиентов об изменениях

  if (!impl->update_lock)
  {
    Notify (NodeEvent_AfterUpdate);
    return;
  }

    //иначе устанавливаем флаг, сигнализирующий о необходимости оповещения клиентов об изменениях по завершении транзакци
    //обновления

  impl->update_notify = true;    
}
