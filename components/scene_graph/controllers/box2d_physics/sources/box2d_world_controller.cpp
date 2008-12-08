#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::box2d;

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
  world.Step (dt, ITERATIONS_COUNT);
}
