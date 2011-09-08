#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации техники рендеринга
*/

struct Technique::Impl
{
  stl::string name; //имя техники
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
    Имя техники
*/

void Technique::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene_render3d::Technique::SetName", "name");
    
  impl->name = name;
}

const char* Technique::Name ()
{
  return impl->name.c_str ();
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
