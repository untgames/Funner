#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace common;

/*
    Вспомогательные структуры
*/

namespace
{

const char* LOG_NAME = "scene_graph.Node";

const bool DEFAULT_SCALE_PIVOT_ENABLED       = true; //значение по умолчанию для использования центра масштабирования
const bool DEFAULT_ORIENTATION_PIVOT_ENABLED = true; //значение по умолчанию для использования центра поворотов

///центр объекта
struct Pivot
{
  bool  pivot_enabled;                          //включён ли центр
  bool  need_local_position_after_pivot_update; //необходимо обновлять локальное положение с учётом центра
  bool  orientation_pivot_enabled;              //включён центр поворотов
  bool  scale_pivot_enabled;                    //включён центр масштабирования
  vec3f pivot_position;                         //локальное положение центра
  vec3f local_position_after_pivot;             //локальное положение с учётом центра
  vec3f world_position_after_pivot;             //мировое положение с учётом центра
  
  Pivot ()
    : pivot_enabled (false)
    , need_local_position_after_pivot_update (false)
    , orientation_pivot_enabled (DEFAULT_ORIENTATION_PIVOT_ENABLED)
    , scale_pivot_enabled (DEFAULT_SCALE_PIVOT_ENABLED)
  {
  }
};

typedef stl::auto_ptr<Pivot>                                                  PivotPtr;
typedef xtl::signal<void (Node& sender, NodeEvent event)>                     NodeSignal;
typedef xtl::signal<void (Node& sender, Node& child, NodeSubTreeEvent event)> SubTreeNodeSignal;
typedef xtl::signal<void (float dt)>                                          UpdateSignal;

/*
    Композиция аффинных преобразований
*/

//композиция преобразований
void affine_compose (const vec3f& position, const quatf& orientation, const vec3f& scale, mat4f& tm)
{
  tm = math::translate (position) * to_matrix (orientation) * math::scale (scale);
}

//декомпозиция аффинных преобразований из матрицы преобразований
void affine_decompose (const math::mat4f& matrix, math::vec3f& position, math::quatf& rotation, math::vec3f& scale)
{
  math::mat4f local_matrix (transpose (matrix)); //копия матрицы преобразований

  //выделение преобразования переноса  
  for (size_t i = 0; i < 3; i++)
  {
    position [i] = local_matrix [3][i];
    local_matrix [3][i] = 0;
  }

  //выделение преобразования масштабирования
  for (size_t i = 0; i < 3; i++)
  {
    //определение длины вектора-столбца преобразований
    float square_length = 0;

    for (size_t j = 0; j < 3; j++)
      square_length += local_matrix [i][j] * local_matrix [i][j];

    scale [i] = sqrt (square_length);

    //нормирование
    for (size_t j = 0; j < 3; j++)
      local_matrix [i][j] /= scale [i];
  }
 
  math::vec3f temp_z = math::cross (math::vec3f (local_matrix [0][0], local_matrix [0][1], local_matrix [0][2]),
      math::vec3f (local_matrix [1][0], local_matrix [1][1], local_matrix [1][2]));

  if (math::dot (temp_z, math::vec3f (local_matrix [2][0], local_matrix [2][1], local_matrix [2][2])) < 0)
  {
    scale.x = -scale.x;
    
    for (size_t j=0; j<3; j++)
      local_matrix [0][j] = -local_matrix [0][j];
  }

//  rotation = to_quat (transpose (local_matrix));
  math::mat4f m = transpose (local_matrix);
  
  m [0] = normalize (m [0]);
  m [2] = normalize (cross (m [0], m [1]));
  m [1] = normalize (cross (m [2], m [0]));
  
  rotation = normalize (to_quat (m));
}


}

/*
    Описание реализации Node
*/

struct Node::Impl
{
  scene_graph::Scene*     scene;                            //сцена, которой принадлежит объект
  stl::string             name;                             //имя узла
  size_t                  name_hash;                        //хэш имени
  size_t                  ref_count;                        //количество ссылок на узел
  Node*                   this_node;                        //текущий узел
  Node*                   parent;                           //родительский узел
  Node*                   first_child;                      //первый потомок
  Node*                   last_child;                       //последний потомок
  Node*                   prev_child;                       //предыдущий потомок
  Node*                   next_child;                       //следующий потомок
  bool                    bind_lock;                        //флаг блокировки на вызов BindToParent
  bool                    need_release_at_unbind;           //нужно ли уменьшать счётчик ссылок узла при отсоединении от родителя
  NodeSignal              signals [NodeEvent_Num];          //сигналы
  bool                    signal_process [NodeEvent_Num];   //флаги обработки сигналов
  SubTreeNodeSignal       subtree_signals [NodeSubTreeEvent_Num]; //сигналы событий, возникающих в узлах-потомках
  bool                    subtree_signal_process [NodeSubTreeEvent_Num]; //флаги обработки сигналов, возникающих в узлах потомках
  PivotPtr                pivot;                            //центр объекта
  vec3f                   local_position;                   //локальное положение
  quatf                   local_orientation;                //локальная ориентация
  vec3f                   local_scale;                      //локальный масштаб
  mat4f                   local_tm;                         //матрица локальных преобразований
  vec3f                   world_position;                   //мировое положение
  quatf                   world_orientation;                //мировая ориентация
  vec3f                   world_scale;                      //мировой масштаб
  mat4f                   world_tm;                         //матрица мировых преобразований
  bool                    orientation_inherit;              //флаг наследования родительской ориентации
  bool                    scale_inherit;                    //флаг наследования родительского масштаба
  bool                    need_world_transform_update;      //флаг, сигнализирующий о необходимости пересчёта мировых преобразований
  bool                    need_world_tm_update;             //флаг, сигнализирующий о необходимости пересчёта матрицы мировых преобразований
  bool                    need_local_tm_update;             //флаг, сигнализирующий о необходимости пересчёта матрицы локальных преобразований
  bool                    need_world_position_update;       //флаг, сигнализирующий о необходимости пересчёта мировой позиции
  bool                    need_world_axises_update;         //флаг, сигнализирующий о необходимости пересчёта мировых осей и масштаба
  size_t                  update_lock;                      //счётчик открытых транзакций обновления
  bool                    update_notify;                    //флаг, сигнализирующий о необходимости оповещения об обновлениях по завершении транзакции обновления
  Node*                   first_updatable_child;            //первый обновляемый потомок
  Node*                   prev_updatable_child;             //предыдущий обновляемый потомок
  Node*                   next_updatable_child;             //следующий обновляемый потомок
  UpdateSignal            update_signal;                    //сигнал обновления узла
  NodeProperties::Pointer properties;                       //свойства узла

  Impl (Node* node) : this_node (node)
  {
    ref_count             = 1;
    scene                 = 0;
    parent                = 0;
    first_child           = 0;
    last_child            = 0;
    prev_child            = 0;
    next_child            = 0;
    update_lock           = 0;
    update_notify         = false;
    name_hash             = strhash (name.c_str ());
    first_updatable_child = 0;
    prev_updatable_child  = 0;
    next_updatable_child  = 0;

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
    need_world_position_update  = false;
    need_world_axises_update    = false;
    
      //очистка массива флагов обработки сигналов
      
    for (size_t i=0; i<NodeEvent_Num; i++)
      signal_process [i] = false;
      
    for (size_t i=0; i<NodeSubTreeEvent_Num; i++)
      subtree_signal_process [i] = false;
      
      //очистка флага блокировки на вызов BindToParent
      
    bind_lock = false;
    need_release_at_unbind = false;
  }
  
  Pivot& GetPivot ()
  {
    if (pivot)
      return *pivot;
      
    pivot = PivotPtr (new Pivot);
    
    return *pivot;
  }
  
  const vec3f& PositionAfterPivot ()
  {
    if (!pivot)
      return local_position;
    
    if (pivot->need_local_position_after_pivot_update)
    {
      if (pivot->pivot_enabled)
      {
        static vec3f default_scale (1.0f);
        static quatf default_orientation;

        const vec3f& pivot_scale       = pivot->scale_pivot_enabled ? default_scale : local_scale;
        const quatf& pivot_orientation = pivot->orientation_pivot_enabled ? default_orientation : local_orientation;

        pivot->local_position_after_pivot = local_position
          + pivot_orientation * (pivot_scale * pivot->pivot_position)
          - local_orientation * (local_scale * pivot->pivot_position);
      }
      else
      {
        pivot->local_position_after_pivot = local_position;
      }
      
      pivot->need_local_position_after_pivot_update = false;
    }
    
    return pivot->local_position_after_pivot;
  }
  
  const vec3f& WorldPositionAfterPivot ()
  {
    if (need_world_position_update)
      UpdateWorldPosition ();
      
    return pivot ? pivot->world_position_after_pivot : world_position;
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
    need_world_position_update  = true;
    need_world_axises_update    = true;
    
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
    
    for (Node* node=first_child; node; node=node->impl->next_child) //!!!
      node->impl->UpdateWorldTransformNotify ();
  }

/*
    Оповещение о необходимости пересчёта мировых преобразований / пересчёт мировых преобразований
*/

  void UpdateLocalTransformNotify ()
  {
    need_local_tm_update = true;
    
    if (pivot)
      pivot->need_local_position_after_pivot_update = true;
    
    UpdateWorldTransformNotify ();
  }
  
  void UpdateWorldPosition ()
  {
    UpdateWorldTransform ();    

    if (need_world_position_update)
    {
      for (size_t i = 0; i < 3; i++)
        world_position [i] = world_tm [i][3];
        
      if (pivot)
        pivot->world_position_after_pivot = world_position;
        
      need_world_position_update = false;        
    }    
  }
  
  void UpdateWorldAxises ()
  {
    UpdateWorldTransform ();    
    
    if (need_world_axises_update)
    {
      math::vec3f position;
    
      affine_decompose (world_tm, position, world_orientation, world_scale);
      
      need_world_axises_update = false;      
    }
  }
  
  void UpdateWorldMatrix ()
  {
    UpdateWorldTransform ();
    
    if (need_world_tm_update)
    {
      affine_compose (pivot ? pivot->world_position_after_pivot : world_position, world_orientation, world_scale, world_tm);
      need_world_tm_update = false;    
    }
  }
  
  void UpdateWorldTransform ()
  {
    bool pivot_enabled = pivot && pivot->pivot_enabled && (pivot->orientation_pivot_enabled || pivot->scale_pivot_enabled);
    
    if (parent)
    {
      world_tm = parent->WorldTM () * this_node->LocalTM ();
      
      if (!orientation_inherit || !scale_inherit || pivot_enabled)
      {
        affine_decompose (world_tm, pivot ? pivot->world_position_after_pivot : world_position, world_orientation, world_scale);
        
        if (!orientation_inherit) world_orientation = local_orientation;
        if (!scale_inherit)       world_scale       = local_scale;
        
        if (pivot)
        {
          if (pivot_enabled)
          {
            static vec3f default_scale (1.0f);
            static quatf default_orientation;

            
            
            const vec3f& pivot_scale       = pivot->scale_pivot_enabled ? scale_inherit ? parent->WorldScale () : default_scale : world_scale;
            const quatf& pivot_orientation = pivot->orientation_pivot_enabled ? orientation_inherit ? parent->WorldOrientation () : default_orientation : world_orientation;
            
            world_position = pivot->world_position_after_pivot - pivot_orientation * (pivot_scale * pivot->pivot_position) + 
                             world_orientation * (world_scale * pivot->pivot_position);
          }
          else
          {
            world_position = pivot->world_position_after_pivot;
          }
        }
        
        need_world_tm_update       = true;
        need_world_position_update = false;
        need_world_axises_update   = false;
      }
      else
      {
        need_world_tm_update       = false;
        need_world_position_update = true;
        need_world_axises_update   = true;
      }      
    }
    else
    {
      world_orientation          = local_orientation;
      world_scale                = local_scale;      
      world_position             = local_position;
      need_world_tm_update       = true;
      need_world_axises_update   = false;
      need_world_position_update = false;

      if (pivot)
      {
        pivot->world_position_after_pivot = PositionAfterPivot ();
      }
    }

    need_world_transform_update = false;
  }

  void BindToParentImpl (Node* parent_node, NodeBindMode mode, NodeTransformSpace invariant_space)
  {
      //защита от вызова Bind в обработчиках соответствующих событий

    if (bind_lock)
      return;

      //определяем инвариантное пространство преобразований
      
    bool  need_transform_in_world_space = false;
    mat4f old_world_tm;

    switch (invariant_space)
    {
      case NodeTransformSpace_Parent:
      case NodeTransformSpace_Local:
        break;
      case NodeTransformSpace_World:
        need_transform_in_world_space  = true;
        old_world_tm                   = this_node->WorldTM ();
        break;
      default:
        throw xtl::make_argument_exception ("scene_graph::Node::BindToParent", "invariant_space", invariant_space);
    }
    
      //проверяем корректность режима присоединения
      
    switch (mode)
    {
      case NodeBindMode_AddRef:
      case NodeBindMode_WeakRef:
        break;
      default:
        throw xtl::make_argument_exception ("scene_graph::Node::BindToParent", "mode", mode);
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
      
        //отсоединяем узел от списка обновлений родителя
        
      UnbindFromParentUpdateList ();
      
        //уменьшаем счётчик ссылок
      
      if (need_release_at_unbind)
      {
        need_release_at_unbind = false;
        this_node->Release ();
      }
    }

      //связываем узел с новым родителем

    parent = parent_node;

    if (parent_node)
    {
        //увеличиваем число ссылок если этого требует режим присоединения

      if (mode == NodeBindMode_AddRef)
      {
        this_node->AddRef ();
        
        need_release_at_unbind = true;
      }
      else
      {
        need_release_at_unbind = false;
      }

        //регистрируем узел в списке потомков родителя
        
      Impl* parent_impl = parent_node->impl;

      prev_child   = parent_impl->last_child;
      next_child   = 0;
      parent_impl->last_child = this_node;

      if (prev_child) prev_child->impl->next_child = this_node;
      else            parent_impl->first_child     = this_node;
      
        //установка текущей сцены
        
      SetScene (parent_impl->scene);

        //добавление в список обновлений родителя

      if (HasUpdatables ())
        BindToParentUpdateList ();

        //оповещение о присоединении узла к родителю

      BindNotify ();
    }
    else
    {
      prev_child = next_child = 0;

        //установка текущей сцены

      SetScene (0);
    }

      //преобразование системы координат
      
    if (need_transform_in_world_space)
    {           
      this_node->BeginUpdate ();
      
      world_tm                    = old_world_tm;
      need_world_transform_update = false;
      need_world_position_update  = false;
      need_world_axises_update    = false;
      need_world_tm_update        = true;
      
      this_node->EndUpdate ();
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

    for (Node* volatile node=first_child; node;) //!!!
    {
      Node* next = node->impl->next_child;

      node->impl->SetScene (scene);
      
      node = next;
    }

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

    for (Node* node=child.impl->first_child; node;) //!!!
    {
      Node* next = node->impl->next_child;

      Notify (*node, event);
      
      node = next;
    }
      
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
  
/*
    Работа со списком обновляемых узлов
*/

  void BindToParentUpdateList ()
  {
    if (!parent || prev_updatable_child)
      return;

    if (parent->impl->first_updatable_child) //если список обновляемых узлов не пуст
    {
      next_updatable_child = parent->impl->first_updatable_child;
      prev_updatable_child = next_updatable_child->impl->prev_updatable_child;

      prev_updatable_child->impl->next_updatable_child = this_node;
      next_updatable_child->impl->prev_updatable_child = this_node;
    }
    else
    {
        //если список обновляемых узлов пуст

      parent->impl->first_updatable_child = this_node;
      prev_updatable_child                = this_node;
      next_updatable_child                = this_node;
      
        //рекурсивная регистрация родителя

      parent->impl->BindToParentUpdateList ();
    }
  }
  
  void UnbindFromParentUpdateList ()
  {
    if (!parent || !prev_updatable_child)
      return;
      
    prev_updatable_child->impl->next_updatable_child = next_updatable_child;
    next_updatable_child->impl->prev_updatable_child = prev_updatable_child;  

    if (this_node == parent->impl->first_updatable_child)
    {
      if (this_node != next_updatable_child)
      {
        parent->impl->first_updatable_child = next_updatable_child;
      }
      else
      {
        parent->impl->first_updatable_child = 0;
        
          //рекурсивная отмена регистрации родителя
          
        if (parent->impl->update_signal.empty ())
          parent->impl->UnbindFromParentUpdateList ();
      }
    }
    
    prev_updatable_child = 0;
    next_updatable_child = 0;
  }
  
  bool HasUpdatables () { return !update_signal.empty () || first_updatable_child; }

/*
    Обновление состояния узла
*/

  void UpdateNode (float dt)
  {
    static const char* METHOD_NAME = "scene_graph::Node::Impl::UpdateNode";

    if (update_signal.empty ())
      return;
    
    try
    {
      this_node->BeginUpdate ();
      
      update_signal (dt);
      
      this_node->EndUpdate ();
    }
    catch (xtl::exception& e)
    {
      common::Log log (LOG_NAME);

      log.Printf ("'%s': node '%s' update exception '%s'", METHOD_NAME, name.c_str (), e.what ());

      //подавление всех исключений
    }
    catch (...)
    {
      common::Log log (LOG_NAME);

      log.Printf ("'%s': node '%s' update unknown exception", METHOD_NAME, name.c_str ());

      //подавление всех исключений
    }
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
    Свойства узла (могут быть NULL)
*/

NodeProperties::Pointer Node::Properties ()
{
  return impl->properties;
}

NodeProperties::ConstPointer Node::Properties () const
{
  return impl->properties;
}

void Node::SetProperties (const NodeProperties::Pointer& properties)
{
  impl->properties = properties;
  
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
  impl->BindToParentImpl (0, NodeBindMode_WeakRef, invariant_space);
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

void Node::Traverse (const TraverseFunction& fn, NodeTraverseMode mode) const
{
  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_OnlyThis:
    case NodeTraverseMode_TopToBottom:
      fn (const_cast<Node&> (*this));
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::Traverse(const TraverseFunction&)", "mode", mode);
  }

  if (mode != NodeTraverseMode_OnlyThis)
  {
    for (Node* node=impl->first_child; node; node=node->impl->next_child) ///!!!
      node->Traverse (fn, mode);
  }
    
  if (mode == NodeTraverseMode_BottomToTop)
    fn (const_cast<Node&> (*this));
}

void Node::Traverse (INodeTraverser& traverser, NodeTraverseMode mode) const
{
  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_OnlyThis:
    case NodeTraverseMode_TopToBottom:
      traverser (const_cast<Node&> (*this));
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::Traverse(INodeTraverser&)", "mode", mode);
      break;
  }

  if (mode != NodeTraverseMode_OnlyThis)
  {
    for (Node* node=impl->first_child; node; node=node->impl->next_child) ///!!!
      node->Traverse (traverser, mode);
  }

  if (mode == NodeTraverseMode_BottomToTop)
    traverser (const_cast<Node&> (*this));
}

void Node::VisitEach (Visitor& visitor, NodeTraverseMode mode) const
{
  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_OnlyThis:
    case NodeTraverseMode_TopToBottom:
      const_cast<Node&> (*this).AcceptCore (visitor);
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::VisitEach", "mode", mode);
      break;
  }

  if (mode != NodeTraverseMode_OnlyThis)
  {
    for (Node* node=impl->first_child; node; node=node->impl->next_child) //!!!
      node->VisitEach (visitor, mode);
  }

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

void Node::SetWorldPosition (const vec3f& position)
{
  Translate (position - WorldPosition (), NodeTransformSpace_World);
}

void Node::SetWorldPosition (float x, float y, float z)
{
  SetWorldPosition (vec3f (x, y, z));
}

const vec3f& Node::Position () const
{
  return impl->local_position;
}

const vec3f& Node::WorldPosition () const
{
  if (impl->need_world_position_update)
    impl->UpdateWorldPosition ();
    
  return impl->world_position;
}

/*
    Центр узла в локальной системе координат (точка применения поворотов и масштаба)
*/

void Node::SetPivotPosition (const math::vec3f& pivot_position)
{
  static float       EPS = 0.001f;
  static math::vec3f ZERO_VEC (0.0f);

  if (equal (pivot_position, ZERO_VEC, EPS))
  {
    ResetPivotPosition ();
  }
  else
  {
    Pivot& pivot = impl->GetPivot ();
    
    pivot.pivot_position = pivot_position;
    pivot.pivot_enabled  = true;
    
    impl->UpdateLocalTransformNotify ();
  }
}

void Node::SetPivotPosition (float x, float y, float z)
{
  SetPivotPosition (math::vec3f (x, y, z));
}

void Node::ResetPivotPosition ()
{
  if (!impl->pivot)
    return;

  Pivot& pivot = impl->GetPivot ();

  pivot.pivot_enabled  = false;
  pivot.pivot_position = math::vec3f (0.0f);
  
  impl->UpdateLocalTransformNotify ();
}

const math::vec3f& Node::PivotPosition () const
{
  static vec3f ZERO_VEC (0.0f);

  return impl->pivot ? impl->pivot->pivot_position : ZERO_VEC;
}

//установка флага применения центра поворотов
void Node::SetOrientationPivotEnabled (bool state)
{
  if (!impl->pivot && state == DEFAULT_ORIENTATION_PIVOT_ENABLED)
    return;
    
  Pivot& pivot = impl->GetPivot ();
  
  pivot.orientation_pivot_enabled = state;
  
  impl->UpdateLocalTransformNotify ();
}

//установка флага применения центра масштаба
void Node::SetScalePivotEnabled (bool state)
{
  if (!impl->pivot && state == DEFAULT_SCALE_PIVOT_ENABLED)
    return;
    
  Pivot& pivot = impl->GetPivot ();
  
  pivot.scale_pivot_enabled = state;
  
  impl->UpdateLocalTransformNotify ();
}

bool Node::OrientationPivotEnabled () const
{
  return impl->pivot ? impl->pivot->orientation_pivot_enabled : DEFAULT_ORIENTATION_PIVOT_ENABLED;
}

bool Node::ScalePivotEnabled () const
{
  return impl->pivot ? impl->pivot->scale_pivot_enabled : DEFAULT_SCALE_PIVOT_ENABLED;
}

//проверка: присутствует ли pivot в данном узле
bool Node::PivotEnabled () const
{
  return impl->pivot && impl->pivot->pivot_enabled && (impl->pivot->orientation_pivot_enabled || impl->pivot->scale_pivot_enabled);
}

/*
    Ориентация узла
*/

void Node::SetOrientation (const quatf& orientation)
{
  impl->local_orientation = orientation;
  
  impl->UpdateLocalTransformNotify ();
}

void Node::SetOrientation (const math::anglef& angle, float axis_x, float axis_y, float axis_z)
{
  SetOrientation (to_quat (angle, vec3f (axis_x, axis_y, axis_z)));
}

void Node::SetOrientation (const math::anglef& pitch, const math::anglef& yaw, const math::anglef& roll)
{
  SetOrientation (to_quat (pitch, yaw, roll));
}

void Node::SetWorldOrientation (const quatf& orientation)
{
//  if (impl->orientation_inherit)
//  {
    Rotate (orientation * inverse (WorldOrientation ()), NodeTransformSpace_World);
//  }
//  else
//  {
//    SetOrientation (orientation);
//  }
}

void Node::SetWorldOrientation (const math::anglef& angle, float axis_x, float axis_y, float axis_z)
{
  SetWorldOrientation (to_quat (angle, vec3f (axis_x, axis_y, axis_z)));
}

void Node::SetWorldOrientation (const math::anglef& pitch, const math::anglef& yaw, const math::anglef& roll)
{
  SetWorldOrientation (to_quat (pitch, yaw, roll));
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
  if (impl->need_world_axises_update)
    impl->UpdateWorldAxises ();
    
  return impl->world_orientation;
}

/*
    Позиционирование узла
*/

void Node::LookTo (const vec3f& target_point, const vec3f& up, NodeTransformSpace space)
{
  math::vec3f x, y, z;

  switch (space)
  {
    case NodeTransformSpace_Local:
      z = normalize (target_point);
      y = normalize (up);

      break;
    case NodeTransformSpace_Parent:
    {
      quatf q = inverse (normalize (Orientation ()));

      z = normalize (q * vec4f (target_point - Position ()));
      y = normalize (q * vec4f (up, 0));

      break;
    }
    case NodeTransformSpace_World:
    {            
      quatf q = inverse (normalize (WorldOrientation ()));

      z = normalize (q * vec4f (target_point - WorldPosition ()));
      y = normalize (q * vec4f (up, 0));

      break;
    }
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::LookTo(const vec3f&, const vec3f&, NodeTransformSpace)", "space", space);
  }
  
  static const float EPS = 0.001f;
  
  if (qlen (y) < EPS || qlen (z) < EPS || equal (y, z, EPS))
    return; //игнорирование позиционирования на начало локальной системы координат

  mat3f view;

  x = cross (y, z);
  y = cross (z, x);

  view [0] = x;
  view [1] = y;
  view [2] = z;
  view     = transpose (view);  
  
  quatf rotation = normalize (to_quat (view));  

  SetOrientation (rotation * impl->local_orientation);
}

void Node::LookTo (const math::vec3f& target_point, NodeOrt direction, NodeOrt invariant, NodeTransformSpace space)
{
  static const char* METHOD_NAME = "scene_graph::Node::LookTo(const vec3f&, NodeOrt, NodeOrt, NodeTransformSpace)";
  
    //приведение целевой точки в локальную систему координат

  vec3f local_dir;

  switch (space)
  {
    case NodeTransformSpace_Local:
      local_dir = normalize (target_point);

      break;
    case NodeTransformSpace_Parent:
    {
      quatf q = inverse (normalize (Orientation ()));

      local_dir = normalize (q * vec4f (target_point - Position ()));

      break;
    }
    case NodeTransformSpace_World:
    {            
      quatf q = inverse (normalize (WorldOrientation ()));      

      local_dir = normalize (q * vec4f (target_point - WorldPosition ()));

      break;
    }
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "space", space);
  }

    //проверка корректности осей
  
  switch (direction)
  {
    case NodeOrt_X:
    case NodeOrt_Y:
    case NodeOrt_Z:
      break;    
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "direction", direction);
  }
  
  switch (invariant)
  {
    case NodeOrt_X:
    case NodeOrt_Y:
    case NodeOrt_Z:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "invariant", invariant);
  }
  
  if (invariant == direction)
    throw xtl::format_operation_exception (METHOD_NAME, "Bad configuration invariant = direction");
    
    //диспетчеризация
    
  switch (direction)
  {
    case NodeOrt_X:
      switch (invariant)
      {
        case NodeOrt_Y:
          LookTo (cross (local_dir, vec3f (0, 1, 0)), vec3f (0, 1, 0), NodeTransformSpace_Local);
          break;
        case NodeOrt_Z:
        {
          LookTo (vec3f (0, 0, 1), cross (vec3f (0, 0, 1), local_dir), NodeTransformSpace_Local);
          break;
        }
        default:
          break;
      }

      break;
    case NodeOrt_Y:
      switch (invariant)
      {
        case NodeOrt_X:
        {
          vec3f x (1, 0, 0),
                z = cross (x, local_dir),
                y = cross (z, x);
          
          LookTo (z, y, NodeTransformSpace_Local);
          break;
        }
        case NodeOrt_Z:
          LookTo (vec3f (0, 0, 1), local_dir, NodeTransformSpace_Local);
          break;
        default:
          break;
      }

      break;
    case NodeOrt_Z:
      switch (invariant)
      {
        case NodeOrt_X:
        {
          vec3f x (1, 0, 0),
                y = cross (local_dir, x),
                z = cross (x, y);

          LookTo (z, y, NodeTransformSpace_Local);
          break;
        }
        case NodeOrt_Y:
        {
          vec3f y (0, 1, 0),
                x = cross (y, local_dir),
                z = cross (x, y);
          
          LookTo (z, y, NodeTransformSpace_Local);
          break;
        }
        default:
          break;
      }
      
      break;
    default:
      break;
  }
}

void Node::LookTo (const math::vec3f& target_point, NodeTransformSpace space)
{
  LookTo (target_point, NodeOrt_Z, NodeOrt_Y, space);
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

void Node::SetWorldScale (const vec3f& scale)
{
//  if (impl->scale_inherit)
//  {
    Scale (scale / WorldScale ());
//  }
//  else
//  {
//    SetScale (scale);
//  }
}

void Node::SetWorldScale (float sx, float sy, float sz)
{
  SetWorldScale (vec3f (sx, sy, sz));
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
  if (impl->need_world_axises_update)
    impl->UpdateWorldAxises ();
    
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
      if (impl->parent) impl->local_position += inverse (impl->parent->WorldOrientation ()) * offset / impl->parent->WorldScale ();
      else              impl->local_position += offset;

      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::Translate", "space", space);
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

      impl->local_orientation = impl->local_orientation * inverse (world_orientation) * q * world_orientation;
      break;
    }      
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::Rotate", "space", space);
      break;
  }

  impl->UpdateLocalTransformNotify ();
}

void Node::Rotate (const math::anglef& angle, float axis_x, float axis_y, float axis_z, NodeTransformSpace space)
{
  Rotate (to_quat (angle, vec3f (axis_x, axis_y, axis_z)), space);
}

void Node::Rotate (const math::anglef& pitch, const math::anglef& yaw, const math::anglef& roll, NodeTransformSpace space)
{
  Rotate (to_quat (pitch, yaw, roll), space);
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

const mat4f& Node::TransformationMatrix (NodeTransformSpace space) const
{
  static mat4f idNode;

  switch (space)
  {
    case NodeTransformSpace_Local:
      if (impl->need_local_tm_update)
      {
        affine_compose (impl->PositionAfterPivot (), impl->local_orientation, impl->local_scale, impl->local_tm);

        impl->need_local_tm_update = false;
      }

      return impl->local_tm;
    case NodeTransformSpace_Parent:
      return impl->parent ? impl->parent->TransformationMatrix (NodeTransformSpace_World) : idNode;
    case NodeTransformSpace_World:
      if (impl->need_world_tm_update)
        impl->UpdateWorldMatrix ();

      return impl->world_tm;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::TransformationMatrix", "space", space);
  } 
}

/*
    Получение осей
*/

vec3f Node::Ort (NodeOrt ort, NodeTransformSpace space) const
{
  static const char* METHOD_NAME = "scene_graph::Node::Ort";

    //получение матрицы преобразований

  const mat4f* tm;
  
  switch (space)
  {
    case NodeTransformSpace_Local:
      tm = &LocalTM ();
      break;
    case NodeTransformSpace_World:
      tm = &WorldTM ();
      break;
    case NodeTransformSpace_Parent:
      tm = &ParentTM ();
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "space", space);
  }
  
    //получение оси
    
  vec4f local_ort;

  switch (ort)
  {
    case NodeOrt_X: local_ort = vec4f (1, 0, 0, 0); break;
    case NodeOrt_Y: local_ort = vec4f (0, 1, 0, 0); break;
    case NodeOrt_Z: local_ort = vec4f (0, 0, 1, 0); break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "ort", ort);
  }  
  
  return normalize (vec3f (*tm * local_ort));
}

/*
    Получение матрицы преобразования узла object в системе координат данного узла
*/

mat4f Node::ObjectTM (Node& object) const
{
  return inverse (WorldTM ()) * object.WorldTM ();
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

/*
    Присоединение / отсоединение контроллера
*/

xtl::connection Node::AttachController (const UpdateHandler& fn)
{
  impl->BindToParentUpdateList ();

  return impl->update_signal.connect (fn);
}

void Node::DetachAllControllers ()
{
  impl->update_signal.disconnect_all ();

  if (!impl->HasUpdatables ()) //если нет обновляемых потомков - отсоединение от родительского списка обновлений
    impl->UnbindFromParentUpdateList ();
}

/*
    Обновление состояния узла и его потомков
*/

void Node::Update (float dt, NodeTraverseMode mode)
{  
    //проверка корректности аргументов

  Pointer lock (this);

  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_OnlyThis:
    case NodeTraverseMode_TopToBottom:
      impl->UpdateNode (dt);
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::Update", "mode", mode);
  }
  
  if (mode != NodeTraverseMode_OnlyThis && impl->first_updatable_child)
  {
    impl->first_updatable_child->Update (dt, mode);
    
    if (impl->first_updatable_child) //проверка нужна на случай удаления контроллера в Update
    {  
      Pointer next_lock;

      for (Node* node=impl->first_updatable_child->impl->next_updatable_child; node!=impl->first_updatable_child;) //!!!
      {
        next_lock = Pointer (node->impl->next_updatable_child);

        node->Update (dt, mode);

        node = next_lock.get ();
      }
    }
  }
  
  if (mode == NodeTraverseMode_BottomToTop)
  {
    impl->UpdateNode (dt);
  }

  if (!impl->HasUpdatables ())
    impl->UnbindFromParentUpdateList ();
}
