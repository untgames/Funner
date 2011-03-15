#include "shared.h"

using namespace physics;

/*
   Физическая сцена
*/

/*
   Конструктор / деструктор / копирование
*/

SceneInternal::SceneInternal (SceneImpl* impl)
  : Scene (impl)
  {}

Scene::Scene (SceneImpl* source_impl)
  : impl (source_impl)
  {}

Scene::Scene (const Scene& source)
  : impl (source.impl)
{
  addref (impl);
}

Scene::~Scene ()
{
  release (impl);
}

Scene& Scene::operator = (const Scene& source)
{
  Scene (source).Swap (*this);

  return *this;
}

/*
   Создание тел
*/

RigidBody Scene::CreateBody (const char* name)
{
  return impl->CreateBody (name);
}

RigidBody Scene::CreateBody (const Shape& shape, float mass)
{
  return impl->CreateBody (shape, mass);
}

/*
   Создание связей между телами
*/

Joint Scene::CreateSphericalJoint (const JointBind& bind1, const JointBind& bind2)
{
  return impl->CreateSphericalJoint (bind1, bind2);
}

Joint Scene::CreateConeTwistJoint (const JointBind& bind1, const JointBind& bind2, float swing_limit_in_degrees, float twist_limit)
{
  return impl->CreateConeTwistJoint (bind1, bind2, swing_limit_in_degrees, twist_limit);
}

Joint Scene::CreateHingeJoint (const JointBind& bind1, const JointBind& bind2)
{
  return impl->CreateHingeJoint (bind1, bind2);
}

Joint Scene::CreatePrismaticJoint (const JointBind& bind1, const JointBind& bind2)
{
  return impl->CreatePrismaticJoint (bind1, bind2);
}

/*
   Управление гравитацией
*/

const math::vec3f& Scene::Gravity () const
{
  return impl->Gravity ();
}

void Scene::SetGravity (const math::vec3f& value)
{
  impl->SetGravity (value);
}

/*
   Параметры симуляции
*/

float Scene::SimulationStep () const
{
  return impl->SimulationStep ();
}

void Scene::SetSimulationStep (float step)
{
  impl->SetSimulationStep (step);
}

/*
   Симуляция
*/
void Scene::PerformSimulation (float dt)
{
  impl->PerformSimulation (dt);
}

/*
  Фильтрация столкновений объектов. Объекты сталкиваются, если:
   - не задан фильтр;
   - задан дефолтный фильтр и collides = true;
   - заданный фильтр возвращает true и collides = true;
   - заданный фильтр возвращает false и collides = false.
  Приоритет фильтрации: чем позже добавлен фильтр, тем выше его приоритет
*/

size_t Scene::AddCollisionFilter (const char* group1_mask, const char* group2_mask, bool collides, const BroadphaseCollisionFilter& filter)
{
  return impl->AddCollisionFilter (group1_mask, group2_mask, collides, filter);
}

void Scene::RemoveCollisionFilter (size_t id)
{
  impl->RemoveCollisionFilter (id);
}

void Scene::RemoveAllCollisionFilters ()
{
  impl->RemoveAllCollisionFilters ();
}

/*
   Обработка столкновений объектов
*/

xtl::connection Scene::RegisterCollisionCallback (const char* group1_mask, const char* group2_mask, CollisionEventType event_type, const CollisionCallback& callback_handler)
{
  return impl->RegisterCollisionCallback (group1_mask, group2_mask, event_type, callback_handler);
}

/*
   Отладочная отрисовка
*/

void Scene::Draw (render::debug::PrimitiveRender& render)
{
  impl->Draw (render);
}

/*
   Обмен
*/

void Scene::Swap (Scene& scene)
{
  stl::swap (impl, scene.impl);
}

namespace physics
{

/*
   Обмен
*/

void swap (Scene& scene1, Scene& scene2)
{
  scene1.Swap (scene2);
}

}
