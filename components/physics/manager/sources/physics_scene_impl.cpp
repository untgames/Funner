#include "shared.h"

using namespace physics;

/*
   Физическая сцена
*/

struct SceneImpl::Impl : public xtl::reference_counter
{
  ScenePtr scene;

  Impl (ScenePtr in_scene)
    : scene (in_scene)
    {}
};

/*
   Конструктор / деструктор / копирование
*/

SceneImpl::SceneImpl (ScenePtr scene)
  : impl (new Impl (scene))
  {}

SceneImpl::SceneImpl (const SceneImpl& source)
  : impl (source.impl)
{
  addref (impl);
}

SceneImpl::~SceneImpl ()
{
  release (impl);
}

SceneImpl& SceneImpl::operator = (const SceneImpl& source)
{
  SceneImpl (source).Swap (*this);

  return *this;
}

/*
   Создание тел
*/

RigidBody SceneImpl::CreateBody (const char* name)
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::CreateBody");
}

RigidBody SceneImpl::CreateBody (const Shape& shape, float mass)
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::CreateBody");
}

/*
   Создание связей между телами
*/

Joint SceneImpl::CreateSphericalJoint (const JointBind& bind1, const JointBind& bind2)
{
  throw xtl::make_not_implemented_exception ("SceneImpl::CreateSphericalJoint");
}

Joint SceneImpl::CreateConeTwistJoint (const JointBind& bind1, const JointBind& bind2, float swing_limit_in_degrees, float twist_limit)
{
  throw xtl::make_not_implemented_exception ("SceneImpl::CreateConeTwistJoint");
}

Joint SceneImpl::CreateHingeJoint (const JointBind& bind1, const JointBind& bind2)
{
  throw xtl::make_not_implemented_exception ("SceneImpl::CreateHingeJoint");
}

Joint SceneImpl::CreatePrismaticJoint (const JointBind& bind1, const JointBind& bind2)
{
  throw xtl::make_not_implemented_exception ("SceneImpl::CreatePrismaticJoint");
}

/*
   Управление гравитацией
*/

const math::vec3f& SceneImpl::Gravity () const
{
  return impl->scene->Gravity ();
}

void SceneImpl::SetGravity (const math::vec3f& value)
{
  impl->scene->SetGravity (value);
}

/*
   Параметры симуляции
*/

float SceneImpl::SimulationStep () const
{
  return impl->scene->SimulationStep ();
}

void SceneImpl::SetSimulationStep (float step)
{
  impl->scene->SetSimulationStep (step);
}

/*
   Симуляция
*/
void SceneImpl::PerformSimulation (float dt)
{
  impl->scene->PerformSimulation (dt);
}

/*
  Фильтрация столкновений объектов. Объекты сталкиваются, если:
   - не задан фильтр;
   - задан дефолтный фильтр и collides = true;
   - заданный фильтр возвращает true и collides = true;
   - заданный фильтр возвращает false и collides = false.
  Приоритет фильтрации: чем позже добавлен фильтр, тем выше его приоритет
*/

size_t SceneImpl::AddCollisionFilter (const char* group1_mask, const char* group2_mask, bool collides, const Scene::BroadphaseCollisionFilter& filter)
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::AddCollisionFilter");
}

void SceneImpl::RemoveCollisionFilter (size_t id)
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::RemoveCollisionFilter");
}

void SceneImpl::RemoveAllCollisionFilters ()
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::RemoveAllCollisionFilters");
}

/*
   Обработка столкновений объектов
*/

xtl::connection SceneImpl::RegisterCollisionCallback (const char* group1_mask, const char* group2_mask, CollisionEventType event_type, const CollisionCallback& callback_handler)
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::RegisterCollisionCallback");
}

/*
   Отладочная отрисовка
*/

void SceneImpl::Draw (render::debug::PrimitiveRender& render)
{
  impl->scene->Draw (render);
}

/*
   Обмен
*/

void SceneImpl::Swap (SceneImpl& scene)
{
  stl::swap (impl, scene.impl);
}
