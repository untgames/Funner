#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации скин меша
*/

namespace
{

const size_t RESERVE_JOINTS_COUNT = 16; //резервируемое количество соединений

struct Joint: public xtl::reference_counter, public INodeListener
{
  Node*       node;                       //узел
  math::mat4f inv_tm;                     //инверсная матрица преобразований
  math::mat4f tm;                         //полная матрица преобразований
  bool        need_update_tm;             //флаг необходимости обновления матрицы полного преобразования
  bool&       need_update_joint_matrices; //флаг необходимости обновления матриц (всегда не нулевой после создания)

///Конструктор
  Joint (bool& in_need_update_joint_matrices)
    : node ()
    , inv_tm (1.0f)
    , tm (1.0f)
    , need_update_tm ()
    , need_update_joint_matrices (in_need_update_joint_matrices)
  {
  }

///Установка узла
  void SetNode (Node* new_node)
  {
    if (node == new_node)
      return;

    if (node)
      node->DetachListener (*this);

    node                       = new_node;
    need_update_tm             = true;
    need_update_joint_matrices = true;

    if (node)
      node->AttachListener (*this);
  }

///Установка матрицы обратного преобразования
  void SetInvMatrix (const math::mat4f& new_inv_tm)
  {
    inv_tm                     = new_inv_tm;
    need_update_tm             = true;
    need_update_joint_matrices = true;
  }

///Получение матрицы полного преобразования
  const math::mat4f& WorldTM ()
  {
    if (!need_update_tm)
      return tm;

    tm             = node ? node->WorldMatrix () * inv_tm : inv_tm;
    need_update_tm = false;

    return tm;
  }

///Оповещение об удалении узла
  void OnDestroy (Node&)
  {
    SetNode (0);
  }

///Оповещение об изменении матрица узла
  void OnWorldMatrixUpdate (Node&)
  {
    need_update_tm             = true;
    need_update_joint_matrices = true;
  }
};

typedef xtl::intrusive_ptr<Joint> JointPtr;
typedef stl::vector<JointPtr>     JointArray;

}

struct SkinMesh::Impl: public INodeListener
{
  JointArray joints;                     //соединения
  bool       need_update_joint_matrices; //флаг необходимости обновления матриц соединений

/// Конструктор
  Impl ()
    : need_update_joint_matrices ()
  {
    joints.reserve (RESERVE_JOINTS_COUNT);
  }

///Получение соединения
  Joint& GetJoint (size_t index)
  {
    if (index >= joints.size ())
      throw xtl::make_range_exception ("", "index", index, joints.size ());

    return *joints [index];
  }
};

/*
    Конструкторы / деструктор
*/

SkinMesh::SkinMesh (RenderManager& render_manager)
  : StaticMesh (render_manager)
  , impl (new Impl)
{
}

SkinMesh::~SkinMesh ()
{
}

/*
    Работа с соединениями
*/

size_t SkinMesh::JointsCount () const
{
  return impl->joints.size ();
}

void SkinMesh::SetJointsCount (size_t count)
{
  size_t old_count = impl->joints.size ();

  if (old_count == count)
    return;

  impl->joints.resize (count);

  impl->need_update_joint_matrices = true;

  if (old_count < count)
  {
    try
    {
      for (size_t i=old_count; i<count; i++)
      {
        JointPtr& joint = impl->joints [i];

        joint = JointPtr (new Joint (impl->need_update_joint_matrices), false);
      }
    }
    catch (...)
    {
      impl->joints.resize (old_count);
      throw;
    }
  }
}

Node* SkinMesh::JointNode (size_t index) const
{
  try
  {
    return impl->GetJoint (index).node;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SkinMesh::JointNode");
    throw;
  }
}

const math::mat4f& SkinMesh::JointInvMatrix (size_t index) const
{
  try
  {
    return impl->GetJoint (index).inv_tm;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SkinMesh::JointInvMatrix");
    throw;
  }
}

void SkinMesh::SetJointNode (size_t index, Node* node)
{
  try
  {
    Joint& joint = impl->GetJoint (index);

    joint.SetNode (node);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SkinMesh::SetJointNode");
    throw;
  }
}

void SkinMesh::SetJointInvMatrix (size_t index, const math::mat4f& tm)
{
  try
  {
    Joint& joint = impl->GetJoint (index);

    joint.SetInvMatrix (tm);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SkinMesh::SetJointInvMatrix");
    throw;
  }
}

/*
    Отрисовка
*/

void SkinMesh::DrawCore (RenderingContext& context, void* user_data)
{
  try
  {
      //обновление соединений

    if (impl->need_update_joint_matrices)
    {
      manager::Entity& entity = Entity ();

      if (entity.JointsCount () != impl->joints.size ())
        entity.SetJointsCount (impl->joints.size ());

      for (JointArray::iterator it=impl->joints.begin (), end=impl->joints.end (); it!=end; ++it)
      {
        Joint&             joint = **it;
        const math::mat4f& tm    = joint.WorldTM ();

        entity.SetJointTransformation (it - impl->joints.begin (), tm);
      }

      impl->need_update_joint_matrices = false;
    }

      //отрисовка

    StaticMesh::DrawCore (context, user_data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SkinMesh::DrawCore");
    throw;
  }
}
