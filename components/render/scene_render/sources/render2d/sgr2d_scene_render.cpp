#include "shared.h"

using namespace render;
using namespace render::render2d;

/*
    Конструктор / деструктор
*/

SceneRender2d::SceneRender2d ()
{
}

SceneRender2d::~SceneRender2d ()
{
}

/*
    Рисование сцены
*/

void SceneRender2d::Render (IRenderContext& render_context)
{
  throw xtl::make_not_implemented_exception ("render::render2d::SceneRender2d::Render");
}

/*
    Подсчёт ссылок
*/

void SceneRender2d::AddRef ()
{
  addref (this);
}

void SceneRender2d::Release ()
{
  release (this);
}
