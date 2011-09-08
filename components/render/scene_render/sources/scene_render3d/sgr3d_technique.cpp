#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации техники рендеринга
*/

struct Technique::Impl
{
};

/*
    Конструктор / деструктор
*/

Technique::Technique ()
  : impl (new Impl)
{
}

Technique::~Technique ()
{
}

/*
    Обновление камеры
*/

void Technique::UpdateCamera (scene_graph::Camera* camera)
{
  throw xtl::make_not_implemented_exception ("render::scene_render3d::Technique::UpdateCamera");
}

/*
    Обновление свойств ренедринга
*/

void Technique::UpdateProperties (const common::PropertyMap&)
{
  throw xtl::make_not_implemented_exception ("render::scene_render3d::Technique::UpdateProperties");
}

/*
    Отрисовка
*/

void Technique::UpdateFrame (Scene& scene, Frame& frame)
{
  throw xtl::make_not_implemented_exception ("render::scene_render3d::Technique::Draw");
}
