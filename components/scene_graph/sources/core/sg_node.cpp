#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace common;

/*
    Описание реализации Node
*/

typedef xtl::signal<void (Node& sender, NodeEvent event)> NodeSignal;
typedef xtl::signal<void (Node& sender, Node& child, NodeSubTreeEvent event)> SubTreeNodeSignal;

struct Node::Impl
{
  scene_graph::Scene* scene;                            //сцена, которой принадлежит объект
  stl::string         name;                             //имя узла
  size_t              name_hash;                        //хэш имени
  size_t              ref_count;                        //количество ссылок на узел
  Node*               this_node;                        //текущий узел
  Node*               parent;                           //родительский узел
  Node*               first_child;                      //первый потомок
  Node*               last_child;                       //последний потомок
  Node*               prev_child;                       //предыдущий потомок
  Node*               next_child;                       //следующий потомок
  bool                bind_lock;                        //флаг блокировки на вызов BindToParent
  NodeSignal          signals [NodeEvent_Num];          //сигналы
  bool                signal_process [NodeEvent_Num];   //флаги обработки сигналов
  SubTreeNodeSignal   subtree_signals [NodeSubTreeEvent_Num]; //сигналы событий, возникающих в узлах-потомках
  bool                subtree_signal_process [NodeSubTreeEvent_Num]; //флаги обработки сигналов, возникающих в узлах потомках
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

  Impl (Node* node) : this_node (node)
  {
    ref_count     = 1;
    scene         = 0;
    parent        = 0;
    first_child   = 0;
    last_child    = 0;
    prev_child    = 0;
    next_child    = 0;
    update_lock   = 0;
    update_notify = false;
    name_hash     = strhash (name.c_str ());

      //масштаб по умолчанию

    local_scale = 1.0f;
    world_scale = 1.0f;

      //по умолчанию узел наследует все преобразования родителя

    orientation_inherit = true;
    scale_inherit       = true;

      //преобразования рассчитаны

    need_world_transform_update = false;
    need_local_tm_update        = false;
    need_world_tm_update        = false;  
    
      //очистка массива флагов обработки сигналов
      
    for (size_t i=0; i<NodeEvent_Num; i++)
      signal_process [i] = false;
      
    for (size_t i=0; i<NodeSubTreeEvent_Num; i++)
      subtree_signal_process [i] = false;
      
      //очистка флага блокировки на вызов BindToParent
      
    bind_lock = false;
  }

  //оповещение об изменении узла
  void UpdateNotify ()
  {
      //если узел не находится в транзакции обновления - оповещаем клиентов об изменениях

    if (!update_lock)
    {
      Notify (NodeEvent_AfterUpdate);
      return;
    }

      //иначе устанавливаем флаг, сигнализирующий о необходимости оповещения клиентов об изменениях по завершении транзакци
      //обновления

    update_notify = true;    
  }

  void UpdateWorldTransformNotify ()
  {
    UpdateNotify ();

    if (need_world_transform_update)
      return;      

    need_world_transform_update = true;
    need_world_tm_update        = true;
    
      //оповещение производных классов об изменении положения объекта

    try
    {
      this_node->AfterUpdateWorldTransformEvent ();
    }
    catch (...)
    {
      //поглощаем все исключения
    }

      //оповещение всех потомков о необходимости пересчёта мировых преобразований
    
    for (Node* node=first_child; node; node=node->impl->next_child)
      node->impl->UpdateWorldTransformNotify ();
  }

/*
    Оповещение о необходимости пересчёта мировых преобразований / пересчёт мировых преобразований
*/

  void UpdateLocalTransformNotify ()
  {
    need_local_tm_update = true;
    
    UpdateWorldTransformNotify ();
  }

  void UpdateWorldTransform ()
  {
    if (parent)
    {
      const quatf& parent_orientation = parent->WorldOrientation ();
      
      if (orientation_inherit) world_orientation = parent_orientation * local_orientation;
      else                     world_orientation = local_orientation;

      const vec3f& parent_scale = parent->WorldScale ();

      if (scale_inherit) world_scale = parent_scale * local_scale;
      else               world_scale = local_scale;
      
      world_position = parent_orientation * (parent_scale * local_position) + parent->WorldPosition ();
    }
    else
    {
      world_orientation = local_orientation;
      world_position    = local_position;
      world_scale       = local_scale;
    }

    need_world_transform_update = false;
  }

  void BindToParentImpl (Node* parent_node, NodeBindMode mode, NodeTransformSpace invariant_space)
  {
      //защита от вызова Bind в обработчиках соответствующих событий

    if (bind_lock)
      return;

      //определяем инвариантное пространство преобразований

    switch (invariant_space)
    {
      case NodeTransformSpace_Parent:
      case NodeTransformSpace_Local:
        break;
      case NodeTransformSpace_World:
        throw xtl::make_not_implemented_exception ("scene_graph::Node::BindToParent (invariant_space=NodeTransformSpace_World)");
        break;
      default:
        throw xtl::make_argument_exception ("scene_graph::Node::BindToParent", "invariant_space", invariant_space);
        break;
    }
    
      //проверяем корректность режима присоединения
      
    switch (mode)
    {
      case NodeBindMode_AddRef:
      case NodeBindMode_Capture:
        break;
      default:
        throw xtl::make_argument_exception ("scene_graph::Node::BindToParent", "mode", mode);
        break;
    }
    
      //если родитель не изменяется нет необходимости в присоединии

    if (parent_node == parent)
      return;
      
      //проверка попытки отсоединения корневого узла от сцены

    if (!parent && scene)
      throw xtl::format_not_supported_exception ("scene_graph::Node::BindToParent", "Attempt to bind scene '%s' root to parent not supproted", scene->Name ());

      //проверка не присоединяется ли узел к своему потомку
      
    for (Node* node = parent_node; node; node=node->impl->parent)
      if (node == this_node)
        throw xtl::make_argument_exception ("scene_graph::Node::BindToParent", "parent", "Attempt to bind object to one of it's child");
        
      //устанавливаем блокировку на вызов BindToParent
      
    bind_lock = true;
    
      //увеличиваем счётчик ссылок
      
    this_node->AddRef ();

      //если у узла уже есть родитель отсоединяем его
      
    if (parent)
    {
        //оповещаем клиентов об отсоединении узла от родителя
      UnbindNotify ();

        //отсоединям узел от родителя    
      
      if (prev_child) prev_child->impl->next_child = next_child;
      else            parent->impl->first_child    = next_child;
      
      if (next_child) next_child->impl->prev_child = prev_child;
      else            parent->impl->last_child     = prev_child;    
      
        //уменьшаем счётчик ссылок
      
      this_node->Release ();
    }

      //связываем узел с новым родителем

    parent = parent_node;

    if (parent_node)
    {
        //увеличиваем число ссылок если этого требует режим присоединения

      if (mode == NodeBindMode_AddRef)
        this_node->AddRef ();

        //регистрируем узел в списке потомков родителя
        
      Impl* parent_impl = parent_node->impl;

      prev_child   = parent_impl->last_child;
      next_child   = 0;
      parent_impl->last_child = this_node;

      if (prev_child) prev_child->impl->next_child = this_node;
      else            parent_impl->first_child     = this_node;
      
        //установка текущей сцены
        
      SetScene (parent_impl->scene);

        //оповещение о присоединении узла к родителю

      BindNotify ();
    }
    else
    {
      prev_child = next_child = 0;

        //установка текущей сцены

      SetScene (0);
    }

      //снятие блокировки на вызов BindToParent

    bind_lock = false;

      //уменьшаем счётчик ссылок

    this_node->Release ();
  }

  //оповещение о присоединении узла к родителю
  void BindNotify ()
  {
      //оповещения об изменениях
    
    this_node->BeginUpdate ();
      
      //родительские преобразования требуют пересчёта

    UpdateWorldTransformNotify ();
      
      //оповещаем клиентов о присоединении узла к новому родителю

    Notify (NodeEvent_AfterBind);

    this_node->EndUpdate ();
    
      //оповещение родительского узла
      
    if (parent)
      parent->impl->BindChildNotify (*this_node);
  }

  //оповещение об отсоединении узла от родителя
  void UnbindNotify ()
  {
    Notify (NodeEvent_BeforeUnbind);
    
    if (parent)
      parent->impl->UnbindChildNotify (*this_node);
  }

  //оповещение о присоединении прямого или косвенного потомка
  void BindChildNotify (Node& child)
  {
    Notify (child, NodeSubTreeEvent_AfterBind);
    
    if (parent)
      parent->impl->BindChildNotify (child);
  }

  //оповещени об отсоединении прямого или косвенного потомка
  void UnbindChildNotify (Node& child)
  {
    Notify (child, NodeSubTreeEvent_BeforeUnbind);
    
    if (parent)
      parent->impl->UnbindChildNotify (child);
  }

/*
    Установка указателя на сцену, к которой присоединён узел
*/

  void SetScene (scene_graph::Scene* in_scene)
  {
      //если сцены совпадают - игнорируем вызов

    if (scene == in_scene)
      return;
      
      //оповещение об отсоединении от сцены
      
    if (scene)
    {          
      try
      {
        this_node->BeforeSceneDetachEvent ();
      }
      catch (...)
      {
        //игнорируем все исключения
      }
      
      Notify (NodeEvent_BeforeSceneDetach);
    }
    
      //установка указателя на новую сцену

    scene = in_scene;
    
      //обновление сцены в потомках

    for (Node* volatile node=first_child; node; node=node->impl->next_child)
      node->impl->SetScene (scene);
      
      //оповещение о присоединии к новой сцене
      
    if (!in_scene)
    {
      Notify (NodeEvent_AfterSceneChange);
      return;
    }

    try
    {
      this_node->AfterSceneAttachEvent ();
    }
    catch (...)
    {
      //игнорируем все исключения
    }

    Notify (NodeEvent_AfterSceneAttach);    
    Notify (NodeEvent_AfterSceneChange);
  }

/*
    Оповещение клиентов о наступлении события
*/

  void Notify (NodeEvent event)
  {
      //если обработчиков нет - оповещение игнорируется
      
    if (!signals [event])
      return;

      //проверяем нет ли рекурсивного вызова

    if (signal_process [event])
      return;
      
      //устанавливаем флаг обработки события

    signal_process [event] = true;
    
      //вызываем обработчики событий

    try
    {
      signals [event] (*this_node, event);
    }
    catch (...)
    {
      //все исключения клиентских обработчиков событий узла поглощаются
    }
    
      //снимаем флаг обработки события
    
    signal_process [event] = false;
  }

  void Notify (Node& child, NodeSubTreeEvent event)
  {
    //если обработчиков нет - оповещение игнорируется

    if (!subtree_signals [event])
      return;

      //проверяем нет ли рекурсивного вызова

    if (subtree_signal_process [event])
      return;
      
      //оповещаем о возникновении события относительно всех потомков child
      
    for (Node* node=child.impl->first_child; node; node=node->impl->next_child)
      Notify (*node, event);    
      
      //устанавливаем флаг обработки события

    subtree_signal_process [event] = true;
    
      //вызываем обработчики событий

    try
    {
      subtree_signals [event] (*this_node, child, event);
    }
    catch (...)
    {
      //все исключения клиентских обработчиков событий узла поглощаются
    }
    
      //снимаем флаг обработки события
    
    subtree_signal_process [event] = false;  
  }
};

/*
    Конструктор / деструктор
*/

Node::Node ()
  : impl (new Impl (this)) 
{
}

Node::~Node ()
{
    //разрываем иерархические связи

  UnbindAllChildren ();
  Unbind ();
  
    //оповещение об удалении узла
    
  impl->Notify (NodeEvent_AfterDestroy);

    //удаляем реализацию узла

  delete impl;
}

/*
    Создание узла
*/

Node::Pointer Node::Create ()
{
  return Pointer (new Node, false);
}

/*
    Сцена, которой принадлежит объект
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
    throw xtl::make_null_argument_exception ("scene_graph::Node::SetName", "name");

  impl->name      = name;
  impl->name_hash = strhash (name);
  
  UpdateNotify ();
}

/*
    Подсчёт ссылок
*/

void Node::AddRef () const
{
  if (impl->ref_count)
    impl->ref_count++;
}

void Node::Release () const
{
    //защита от повторного удаления в обработчике

  if (impl->signal_process [NodeEvent_BeforeDestroy] || !impl->ref_count)
    return;

  if (!--impl->ref_count)
  {
      //оповещаем клиентов об удалении узла

    impl->Notify (NodeEvent_BeforeDestroy);

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

Node::Pointer Node::Parent ()
{
  return impl->parent;
}

Node::ConstPointer Node::Parent () const
{
  return impl->parent;
}

Node::Pointer Node::FirstChild ()
{
  return impl->first_child;
}

Node::ConstPointer Node::FirstChild () const
{
  return impl->first_child;
}

Node::Pointer Node::LastChild ()
{
  return impl->last_child;
}

Node::ConstPointer Node::LastChild () const
{
  return impl->last_child;
}

Node::Pointer Node::PrevChild ()
{
  return impl->prev_child;
}

Node::ConstPointer Node::PrevChild () const
{
  return impl->prev_child;
}

Node::Pointer Node::NextChild ()
{
  return impl->next_child;
}

Node::ConstPointer Node::NextChild () const
{
  return impl->next_child;
}

/*
    Присоединение узла к родителю
*/

void Node::Unbind (NodeTransformSpace invariant_space)
{
  impl->BindToParentImpl (0, NodeBindMode_Capture, invariant_space);
}

void Node::BindToParent (Node& parent, NodeBindMode mode, NodeTransformSpace invariant_space)
{
  impl->BindToParentImpl (&parent, mode, invariant_space);
}

void Node::BindToScene (scene_graph::Scene& scene, NodeBindMode mode, NodeTransformSpace invariant_space)
{
  BindToParent (scene.Root (), mode, invariant_space);
}

void Node::UnbindChild (const char* name, NodeTransformSpace invariant_space)
{
  UnbindChild (name, NodeSearchMode_OnNextSublevel, invariant_space);
}

void Node::UnbindChild (const char* name, NodeSearchMode mode, NodeTransformSpace invariant_space)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::Node::UnbindChild", "name");

  Node::Pointer child = FindChild (name, mode);
  
  if (!child)
    return;

  child->Unbind (invariant_space);
}

void Node::UnbindAllChildren ()
{
  while (impl->last_child)
    impl->last_child->Unbind ();
}

/*
    Поиск потомка по имени
*/

Node::Pointer Node::FindChild (const char* name, NodeSearchMode mode) //no throw
{
  return xtl::const_pointer_cast<Node> (const_cast<const Node&> (*this).FindChild (name, mode));
}

Node::ConstPointer Node::FindChild (const char* name, NodeSearchMode mode) const //no throw
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::Node::FindChild", "name");    
    
  size_t name_hash = strhash (name);
    
  switch (mode)
  {
    case NodeSearchMode_OnNextSublevel:
      for (const Node* node=impl->first_child; node; node=node->impl->next_child)
        if (node->impl->name_hash == name_hash && node->impl->name == name)
          return node;

      break;
    case NodeSearchMode_OnAllSublevels:
      for (const Node* node=impl->first_child; node; node=node->impl->next_child)
      {
        if (node->impl->name_hash == name_hash && name == node->impl->name)
          return node;

        Node::ConstPointer child = node->FindChild (name, mode);

        if (child)
          return child;
      }

      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::FindChild", "mode", mode);
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
      throw xtl::make_argument_exception ("scene_graph::Node::Traverse", "mode", mode);
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
      throw xtl::make_argument_exception ("scene_graph::Node::Traverse", "mode", mode);
      break;
  }

  for (const Node* node=impl->first_child; node; node=node->impl->next_child)
    node->Traverse (fn, mode);

  if (mode == NodeTraverseMode_BottomToTop)
    fn (*this);
}

void Node::VisitEach (Visitor& visitor, NodeTraverseMode mode) const
{
  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_TopToBottom:
      const_cast<Node&> (*this).AcceptCore (visitor);
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::VisitEach", "mode", mode);
      break;
  }

  for (const Node* node=impl->first_child; node; node=node->impl->next_child)
    node->VisitEach (visitor, mode);

  if (mode == NodeTraverseMode_BottomToTop)
    const_cast<Node&> (*this).AcceptCore (visitor);
}

/*
    Положение узла
*/

void Node::SetPosition (const vec3f& position)
{
  impl->local_position = position;

  impl->UpdateLocalTransformNotify ();
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
    impl->UpdateWorldTransform ();
    
  return impl->world_position;
}

/*
    Ориентация узла
*/

void Node::SetOrientation (const quatf& orientation)
{
  impl->local_orientation = orientation;
  
  impl->UpdateLocalTransformNotify ();
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
    impl->UpdateWorldTransform ();
    
  return impl->world_orientation;
}

/*
    Позиционирование узла
*/

void Node::LookTo (const vec3f& target_point, NodeTransformSpace space)
{
  vec3f local_target_point;

  switch (space)
  {
    case NodeTransformSpace_Local:
      local_target_point = target_point;
      break;
    case NodeTransformSpace_Parent:
      local_target_point = invert (ParentTM ()) * target_point;
      break;
    case NodeTransformSpace_World:
      local_target_point = invert (WorldTM ()) * target_point;
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::LookTo", "space", space);
  }


}

void Node::LookTo (const vec3f& target_point, const vec3f& up, NodeTransformSpace space)
{
  SetOrientation ();
}

void Node::LookAt (const vec3f& position, const vec3f& target, const vec3f& up, NodeTransformSpace space)
{
  LookTo      (target, up, space);
  SetPosition (position); //????
}

void Node::LookTo (float target_x, float target_y, float target_z, NodeTransformSpace space)
{
  LookTo (vec3f (target_x, target_y, target_z));
}

void Node::LookTo (float target_x, float target_y, float target_z, float up_x, float up_y, float up_z, NodeTransformSpace space)
{
  LookTo (vec3f (target_x, target_y, target_z), vec3f (up_x, up_y, up_z), space);
}

void Node::LookAt
 (float              position_x,
  float              position_y,
  float              position_z,
  float              target_x,
  float              target_y,
  float              target_z,
  float              up_x,
  float              up_y,
  float              up_z,
  NodeTransformSpace space)
{
  LookAt (vec3f (position_x, position_y, position_z), vec3f (target_x, target_y, target_z), vec3f (up_x, up_y, up_z), space);
}

/*
    Масштаб узла
*/

void Node::SetScale (const vec3f& scale)
{
  impl->local_scale = scale;
  
  impl->UpdateLocalTransformNotify ();
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
    impl->UpdateWorldTransform ();
    
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

  impl->UpdateWorldTransformNotify ();
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

  impl->UpdateWorldTransformNotify ();
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
      throw xtl::make_argument_exception ("scene_graph::Node::Translate", "space", space);
      break;
  }
  
  impl->UpdateLocalTransformNotify ();
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
      throw xtl::make_argument_exception ("scene_graph::Node::Rotate", "space", space);
      break;
  }

  impl->UpdateLocalTransformNotify ();
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
  
  impl->UpdateLocalTransformNotify ();
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
      throw xtl::make_argument_exception ("scene_graph::Node::TransformationMatrix", "space", space);
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

xtl::connection Node::RegisterEventHandler (NodeEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= NodeEvent_Num)
    throw xtl::make_argument_exception ("scene_graph::Node::Event(NodeEvent)", "event", event);

  return impl->signals [event].connect (handler);
}

xtl::connection Node::RegisterEventHandler (NodeSubTreeEvent event, const SubTreeEventHandler& handler) const
{
  if (event < 0 || event >= NodeSubTreeEvent_Num)
    throw xtl::make_argument_exception ("scene_graph::Node::Event(NodeSubTreeEvent)", "event", event);

  return impl->subtree_signals [event].connect (handler);
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
    throw xtl::format_not_supported_exception ("scene_graph::Node::EndUpdate", "Attempt to call EndUpdate without previous BeginUpdate call");  
    
  if (!--impl->update_lock)
  {
      //если во время транзакции обновления узел изменялся - оповещаем клиентов об изменениях
    
    if (impl->update_notify)
    {
      impl->Notify (NodeEvent_AfterUpdate);
      
      impl->update_notify = false;
    }
  }
}

bool Node::IsInUpdateTransaction () const
{
  return impl->update_lock > 0;  
}

//оповещение об изменении узла
void Node::UpdateNotify () const
{
  impl->UpdateNotify ();
}

void Node::SetScene (scene_graph::Scene* in_scene)
{
  impl->SetScene (in_scene);
}
