#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::physics;
using namespace scene_graph::physics::box2d;

namespace
{

/*
    Константы
*/

const size_t ITERATIONS_COUNT = 10; //количество итераций на шаг расчёта физического мира - в будущем вынести в ресурс

/*
    Утилиты
*/

b2AABB create_bound_box (const b2Vec2& vmin, const b2Vec2& vmax)
{
  b2AABB box = {vmin, vmax};

  return box;    
}

}

/*
    Конструктор / деструктор
*/

WorldController::WorldController (Node& root)
  : world (create_bound_box (b2Vec2 (-100.0f, -100.0f), b2Vec2 (100.0f, 100.0f)), b2Vec2 (0, -10.0f), false),
    root_node (root)
{
  on_attach_child = root.RegisterEventHandler (NodeSubTreeEvent_AfterBind,    xtl::bind (&WorldController::OnAttachChild, this, _2));
  on_detach_child = root.RegisterEventHandler (NodeSubTreeEvent_BeforeUnbind, xtl::bind (&WorldController::OnDetachChild, this, _2));

  printf ("WorldController::WorldController\n");
}

WorldController::~WorldController ()
{
  printf ("WorldController::~WorldController\n");
}

/*
    Обновление мира
*/

void WorldController::Update (float dt)
{
  printf ("WorldController::Update(%.2f)\n", dt);
  world.Step (dt, ITERATIONS_COUNT);
}

/*
    Оповещения о появлении нового узла
*/

void WorldController::OnAttachChild (Node& node)
{
  if (node.Parent () != &root_node)
    return;
    
  Body* body = dynamic_cast<Body*> (&node);
  
  if (!body)
    return;
    
  printf ("create body '%s'\n", body->Name ());
}

void WorldController::OnDetachChild (Node& node)
{
  if (node.Parent () != &root_node)
    return;
}
