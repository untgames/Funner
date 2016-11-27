#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

/*
    Описание реализации контроллера воды
*/

struct DefaultController::Impl: public xtl::instance_counter<DefaultController>
{
  UpdateFunction updater; //функция обновления контроллера
  
  Impl (const UpdateFunction& in_updater) : updater (in_updater) {}
};

/*
    Конструктор / деструктор
*/

DefaultController::DefaultController (Node& node, const UpdateFunction& updater)
  : Controller (node)
  , impl (new Impl (updater))
{
  NodeOwnsController ();
}

DefaultController::~DefaultController ()
{
}

/*
    Создание контроллера
*/

DefaultController::Pointer DefaultController::Create (Node& node)
{
  return Pointer (new DefaultController (node, UpdateFunction ()), false);
}

DefaultController::Pointer DefaultController::Create (Node& node, const UpdateFunction& updater)
{
  return Pointer (new DefaultController (node, updater), false);
}

/*
    Обновление
*/

void DefaultController::Update (const TimeValue& time)
{
  if (impl->updater)
    impl->updater (time);
}
