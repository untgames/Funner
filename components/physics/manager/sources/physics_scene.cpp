#include "shared.h"

using namespace physics;

/*
   Физическая сцена
*/

struct Scene::Impl : public xtl::reference_counter
{
  ScenePtr scene;

  Impl (ScenePtr in_scene)
    : scene (in_scene)
    {}

  ///Создание тел
  RigidBody CreateBody (const char* name)
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::CreateBody");
  }

  RigidBody CreateBody (const Shape& shape, float mass)
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::CreateBody");
  }

  ///Создание связей между телами
  Joint CreateSphericalJoint (const JointBind& bind1, const JointBind& bind2)
  {
    throw xtl::make_not_implemented_exception ("SceneImpl::CreateSphericalJoint");
  }

  Joint CreateConeTwistJoint (const JointBind& bind1, const JointBind& bind2, float swing_limit_in_degrees, float twist_limit)
  {
    throw xtl::make_not_implemented_exception ("SceneImpl::CreateConeTwistJoint");
  }

  Joint CreateHingeJoint (const JointBind& bind1, const JointBind& bind2)
  {
    throw xtl::make_not_implemented_exception ("SceneImpl::CreateHingeJoint");
  }

  Joint CreatePrismaticJoint (const JointBind& bind1, const JointBind& bind2)
  {
    throw xtl::make_not_implemented_exception ("SceneImpl::CreatePrismaticJoint");
  }

  /*
    Фильтрация столкновений объектов. Объекты сталкиваются, если:
     - не задан фильтр;
     - задан дефолтный фильтр и collides = true;
     - заданный фильтр возвращает true и collides = true;
     - заданный фильтр возвращает false и collides = false.
    Приоритет фильтрации: чем позже добавлен фильтр, тем выше его приоритет
  */

  size_t AddCollisionFilter (const char* group1_mask, const char* group2_mask, bool collides, const Scene::BroadphaseCollisionFilter& filter)
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::AddCollisionFilter");
  }

  void RemoveCollisionFilter (size_t id)
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::RemoveCollisionFilter");
  }

  void RemoveAllCollisionFilters ()
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::RemoveAllCollisionFilters");
  }

  /*
     Обработка столкновений объектов
  */

  xtl::connection RegisterCollisionCallback (const char* group1_mask, const char* group2_mask, CollisionEventType event_type, const CollisionCallback& callback_handler)
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::RegisterCollisionCallback");
  }

};

/*
   Конструктор / деструктор / копирование
*/

Scene::Scene (Impl* source_impl)
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
  return impl->scene->Gravity ();
}

void Scene::SetGravity (const math::vec3f& value)
{
  impl->scene->SetGravity (value);
}

/*
   Параметры симуляции
*/

float Scene::SimulationStep () const
{
  return impl->scene->SimulationStep ();
}

void Scene::SetSimulationStep (float step)
{
  impl->scene->SetSimulationStep (step);
}

/*
   Симуляция
*/
void Scene::PerformSimulation (float dt)
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
  impl->scene->Draw (render);
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

/*
   Создание
*/

Scene SceneImplProvider::CreateScene (ScenePtr scene)
{
  return Scene (new Scene::Impl (scene));
}
