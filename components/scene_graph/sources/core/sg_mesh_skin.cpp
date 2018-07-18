#include "shared.h"

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    Описание реализации SkinMesh
*/

namespace
{

const size_t RESERVE_JOINTS_COUNT = 16;

struct Joint;

///Владелец соединений
class IJointOwner
{
  public:
///Оповещение об обновлении соединений
    virtual void OnJointNodeUpdate (Joint& joint) = 0;
    virtual void OnJointMatrixUpdate (Joint& joint) = 0;

  protected:
    virtual ~IJointOwner () {}
};

///Соединение
struct Joint: public xtl::reference_counter
{
  IJointOwner&         owner;                       //владелец соединения
  stl::string          name;                        //имя соединения
  size_t               name_hash;                   //хэш имени
  math::mat4f          inv_tm;                      //инверсная матрица преобразования соединения
  Node*                node;                        //узел
  bool                 need_update;                 //флаг необходимости обновления узла  
  xtl::auto_connection on_before_unbind_connection; //соединение оповещения об отсоединении узла от родителя
  xtl::auto_connection on_update_connection;        //соединение оповещения об обновлении узла (имя)

///Конструктор
  Joint (IJointOwner& in_owner) : owner (in_owner), name (), name_hash (strhash (name.c_str ())), inv_tm (1.0f), node (), need_update () {}

///Установка имени
  void SetName (const char* in_name)
  {
    if (name == in_name)
      return;

    name      = in_name;
    name_hash = strhash (name.c_str ());
    node      = 0;
    
    owner.OnJointNodeUpdate (*this);

    on_before_unbind_connection.disconnect ();
    on_update_connection.disconnect ();
  }

///Установка матрицы обратного преобразования
  void SetInvMatrix (const math::mat4f& tm)
  {
    inv_tm = tm;

    owner.OnJointMatrixUpdate (*this);
  }

///Обновление узла
  void UpdateNode (Node& parent, bool force_update = false)
  {
    if (!need_update && !force_update)
      return;

    if (node)
    {
      on_before_unbind_connection.disconnect ();
      on_update_connection.disconnect ();
    }

    Node::Pointer child = parent.FindChild (name.c_str (), NodeSearchMode_OnAllSublevels);

    node = child.get ();

    if (node)
    {
      try
      {
        on_before_unbind_connection = node->RegisterEventHandler (NodeEvent_BeforeUnbind, xtl::bind (&Joint::OnBeforeUnbind, this));
        on_update_connection = node->RegisterEventHandler (NodeEvent_AfterUpdate, xtl::bind (&Joint::OnAfterUpdate, this));

        need_update = false;
      }
      catch (...)
      {
        node = 0;

        on_before_unbind_connection.disconnect ();
        on_update_connection.disconnect ();

        throw;
      }
    }
  }

///Обработчик оповещения об отсоединении узла от родителя
  void OnBeforeUnbind ()
  {
    node        = 0;
    need_update = true;

    on_before_unbind_connection.disconnect ();
    on_update_connection.disconnect ();
    
    owner.OnJointNodeUpdate (*this);
  }

///Обработчик оповещения об изменении узла
  void OnAfterUpdate ()
  {
    if (!node || node->NameHash () == name_hash)
      return;

    node        = 0;
    need_update = true;

    on_before_unbind_connection.disconnect ();
    on_update_connection.disconnect ();
    
    owner.OnJointNodeUpdate (*this);
  }

///Получение узла
  Node* GetNode (Node& parent)
  {
    if (need_update)
      UpdateNode (parent);

    return node;
  }
};

typedef xtl::intrusive_ptr<Joint>                                    JointPtr;
typedef stl::vector<JointPtr>                                        JointArray;
typedef xtl::signal<void (SkinMesh& sender, SkinMeshEvent event_id)> SkinMeshSignal;

}

struct SkinMesh::Impl: public xtl::instance_counter<StaticMesh>, public IJointOwner
{
  SkinMesh&            owner;                       //обратная ссылка на владельца
  JointArray           joints;                      //соединения
  bool                 joints_need_update;          //соединения требуют обновления
  xtl::auto_connection on_child_bind_connection;    //соединение оповещения о добавлении нового дочернего узла
  SkinMeshSignal       signals [SkinMeshEvent_Num]; //сигналы скин меша

///Конструктор
  Impl (SkinMesh& in_owner)
    : owner (in_owner)
    , joints_need_update ()
  {
    joints.reserve (RESERVE_JOINTS_COUNT);
  }

///Получение соединения
  ::Joint& Joint (size_t index)
  {
    if (index >= joints.size ())
      throw xtl::make_range_exception ("", "index", index, joints.size ());

    return *joints [index];
  }

///Обновление соединений
  void UpdateJoints (Node& parent)
  {
    if (!joints_need_update)
      return;

    for (JointArray::iterator it=joints.begin (), end=joints.end (); it!=end; ++it)
    {
      ::Joint& joint = **it;

      joint.UpdateNode (parent, true);
    }

    joints_need_update = false;
  }

///Оповещение об обновлении соединений
  void OnJointNodeUpdate (::Joint&)
  {
    Notify (owner, SkinMeshEvent_AfterJointNodesUpdate);
  }

  void OnJointMatrixUpdate (::Joint&)
  {
    Notify (owner, SkinMeshEvent_AfterJointMatricesUpdate);
  }

///Оповещение о событии
  void Notify (SkinMesh& sender, SkinMeshEvent event_id)
  {
    try
    {
      if (!signals [event_id])
        return;

      signals [event_id] (sender, event_id);

      switch (event_id)
      {
        case SkinMeshEvent_AfterJointNodesUpdate:
        case SkinMeshEvent_AfterJointMatricesUpdate:
          owner.UpdateNotify ();
          break;
        default:
          break;
      }
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

/*
    Конструктор / деструктор
*/

SkinMesh::SkinMesh ()
  : impl (new Impl (*this))
{
  impl->on_child_bind_connection = RegisterEventHandler (NodeSubTreeEvent_AfterBind, xtl::bind (&SkinMesh::OnChildNodeBind, this, _2));
}

SkinMesh::~SkinMesh ()
{
  delete impl;
}

/*
    Создание модели
*/

SkinMesh::Pointer SkinMesh::Create ()
{
  return Pointer (new SkinMesh, false);
}

/*
    Работа с соединениями
*/

void SkinMesh::SetJointsCount (size_t count)
{
  size_t current_size = impl->joints.size ();

  impl->joints.resize (count);

  try
  {
    for (JointArray::iterator it=impl->joints.begin (), end=impl->joints.end (); it!=end; ++it)
    {
      if (!*it)
        *it = JointPtr (new ::Joint (*impl), false);
    }

    impl->Notify (*this, SkinMeshEvent_AfterJointNodesUpdate);
    impl->Notify (*this, SkinMeshEvent_AfterJointMatricesUpdate);
  }
  catch (...)
  {
    impl->joints.resize (current_size);
    throw;
  }
}

size_t SkinMesh::JointsCount () const
{
  return impl->joints.size ();
}

void SkinMesh::SetJointName (size_t index, const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl->Joint (index).SetName (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SkinMesh::SetJointName");
    throw;
  }
}

void SkinMesh::SetJointInvMatrix (unsigned int index, const math::mat4f& inv_tm)
{
  try
  {
    impl->Joint (index).SetInvMatrix (inv_tm);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SkinMesh::SetJointInvMatrix");
    throw;
  }
}

Node::Pointer SkinMesh::Joint (size_t index)
{
  try
  {
    impl->UpdateJoints (*this);

    return impl->Joint (index).GetNode (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SkinMesh::Joint");
    throw;
  }
}

Node::ConstPointer SkinMesh::Joint (size_t index) const
{
  return const_cast<SkinMesh&> (*this).Joint (index);
}

const char* SkinMesh::JointName (size_t index) const
{
  try
  {
    return impl->Joint (index).name.c_str ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SkinMesh::JointName");
    throw;
  }
}

const math::mat4f& SkinMesh::JointInvMatrix (unsigned int index) const
{
  try
  {
    return impl->Joint (index).inv_tm;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SkinMesh::JointInvMatrix");
    throw;
  }
}

/*
    Оповещение о появлении новых дочерних узлов
*/

void SkinMesh::OnChildNodeBind (Node&)
{
  impl->joints_need_update = true;

  impl->Notify (*this, SkinMeshEvent_AfterJointNodesUpdate);
}

/*
    Подписка на события скин меша
*/

xtl::connection SkinMesh::RegisterEventHandler (SkinMeshEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case SkinMeshEvent_AfterJointNodesUpdate:
    case SkinMeshEvent_AfterJointMatricesUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::SkinMesh::RegisterEventHandler", "event", event);
  }

  return impl->signals [event].connect (handler);
}

/*
    Метод, вызываемый при посещении объекта
*/

void SkinMesh::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    StaticMesh::AcceptCore (visitor);
}

/*
    Связывание свойств
*/

void SkinMesh::BindProperties (common::PropertyBindingMap& bindings)
{
  StaticMesh::BindProperties (bindings);

  bindings.AddProperty ("JointsCount", xtl::bind (&SkinMesh::JointsCount, this), xtl::bind (&SkinMesh::SetJointsCount, this, _1));
}
