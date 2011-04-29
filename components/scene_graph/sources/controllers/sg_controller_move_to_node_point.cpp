#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

/*
    Описание реализации контроллера следования за точкой узла
*/

struct MoveToNodePoint::Impl
{
  Node*                node;                    //передвигаемый узел
  AccelerationFunction acceleration_function;   //функция рассчета ускорения узла
  Node::ConstPointer   target_node;             //преследуемый узел
  math::vec3f          target_point;            //целевая точка в системе координат преследуемого узла
  math::vec3f          current_speed;           //текущая скорость узла
  NodeTransformSpace   transform_space;         //пространство рассчета скорости
  xtl::auto_connection scene_attach_connection; //соединения события присоединения узла к сцене

  Impl (Node& in_node)
    : node (&in_node)
    , current_speed (0.f)
    , transform_space (NodeTransformSpace_Local)
  {
    scene_attach_connection = node->RegisterEventHandler (NodeEvent_AfterSceneAttach, xtl::bind (&MoveToNodePoint::Impl::OnSceneAttach, this));
  }

  void OnSceneAttach ()
  {
    current_speed = 0.f;
  }
};

/*
    Конструктор / деструктор
*/

MoveToNodePoint::MoveToNodePoint (Node& node)
  : Controller (node)
  , impl (new Impl (node))
  {}

MoveToNodePoint::~MoveToNodePoint ()
{
}

/*
    Создание контроллера
*/

MoveToNodePoint::Pointer MoveToNodePoint::Create (Node& node)
{
  return Pointer (new MoveToNodePoint (node), false);
}

/*
   Установка/получение функции рассчета ускорения узла
*/

void MoveToNodePoint::SetAccelerationHandler (const AccelerationFunction& acceleration_function)
{
  impl->acceleration_function = acceleration_function;
}

const MoveToNodePoint::AccelerationFunction& MoveToNodePoint::AccelerationHandler () const
{
  return impl->acceleration_function;
}

/*
   Установка / получение пространства рассчета скорости
*/

void MoveToNodePoint::SetTransformSpace (NodeTransformSpace transform_space)
{
  switch (transform_space)
  {
    case NodeTransformSpace_Local:
    case NodeTransformSpace_World:
      impl->transform_space = transform_space;
      return;
    default:
      throw xtl::make_argument_exception ("scene_graph::controllers::MoveToNodePoint", "transform_space", transform_space);
  }
}

NodeTransformSpace MoveToNodePoint::TransformSpace () const
{
  return impl->transform_space;
}

/*
   Запуск движения
*/

void MoveToNodePoint::Start (Node::ConstPointer node, const math::vec3f& node_space_position)
{
  impl->target_node  = node;
  impl->target_point = node_space_position;
}

/*
   Остановка движения
*/

void MoveToNodePoint::Stop ()
{
  impl->target_node = 0;
}

/*
    Обновление
*/

void MoveToNodePoint::Update (float dt)
{
  static const char* METHOD_NAME = "scene_graph::controllers::MoveToNodePoint::Update";

  if (!impl->node->Scene ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't move node, node is not attached to any scene");

  if (impl->target_node && !impl->target_node->Scene ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't move node, target node is not attached to any scene");

  if (!impl->acceleration_function)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't move node, empty acceleration function");

  math::vec3f current_position = impl->node->Position (),
              acceleration;

  if (impl->target_node)
  {
    math::vec3f move_direction = (impl->target_node->WorldPosition () + impl->target_node->WorldTM () * impl->target_point) - impl->node->WorldPosition ();

    if (impl->transform_space == NodeTransformSpace_Local)
      move_direction = math::inverse (impl->node->WorldTM ()) * move_direction;

    acceleration = impl->acceleration_function (impl->current_speed, move_direction, dt);
  }
  else
    acceleration = impl->acceleration_function (impl->current_speed, 0, dt);

  impl->node->Translate (impl->current_speed * dt + acceleration * dt * dt / 2.f, impl->transform_space);

  impl->current_speed += acceleration * dt;
}
