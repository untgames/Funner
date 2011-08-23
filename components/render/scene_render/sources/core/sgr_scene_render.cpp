#include "shared.h"

using namespace render;

/*
    Описание реализации рендера сцены
*/

struct SceneRender::Impl: public xtl::reference_counter, public scene_graph::IScreenListener, public scene_graph::IViewportListener
{
  render::RenderManager* manager; //менеджер рендеринга связанный с ренедром сцены
  scene_graph::Screen*   screen;  //экран
  
///Конструктор
  Impl ()
    : manager (0)
    , screen (0)
  {
  }
  
///Параметры заднего фона экрана изменены
  void OnScreenChangeBackground (bool state, const math::vec4f& new_color)
  {
  }

///Присоединена область вывода
  void OnScreenAttachViewport (Viewport& viewport)
  {
  }

///Отсоединена область вывода
  void OnScreenDetachViewport (Viewport& viewport)
  {
  }

///Экран удален
  void OnScreenDestroy ()
  {
  }
  
///Порядок вывода изменен
  void OnViewportChangeZOrder (int)
  {
  }
};

/*
    Конструктор / деструктор
*/

SceneRender::SceneRender ()
  : impl (new Impl)
{
}

SceneRender::SceneRender (const SceneRender& render)
  : impl (render.impl)
{
  addref (impl);
}

SceneRender::~SceneRender ()
{
  release (impl);
}

SceneRender& SceneRender::operator = (const SceneRender& render)
{
  SceneRender (render).Swap (*this);
  
  return *this;
}

/*
    Установка / сброс системы рендеринга
*/

void SceneRender::SetRenderManager (render::RenderManager* manager)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::SetRenderManager");
}

render::RenderManager* SceneRender::RenderManager () const
{
  return impl->manager;
}

/*
    Проверка связи цели рендеринга с рендером
*/

bool SceneRender::IsBindedToRenderer () const
{
  return impl->manager && impl->screen;
}

/*
    Экран (политика владения - weak-ref)
*/

void SceneRender::SetScreen (scene_graph::Screen* screen)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::SetScreen");
}

scene_graph::Screen* SceneRender::Screen () const
{
  return impl->screen;
}

/*
    Регистрация целевых буферов отрисовки
*/

void SceneRender::SetRenderTarget (const char* name, const render::RenderTarget& target)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::SetRenderTarget");
}

void SceneRender::RemoveRenderTarget (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::RemoveRenderTarget");
}

void SceneRender::RemoveAllRenderTargets ()
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::RemoveAllRenderTargets");
}

/*
    Получение целевых буферов отрисовки
*/

bool SceneRender::HasRenderTarget (const char* name) const
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::HasRenderTarget");
}

RenderTarget SceneRender::RenderTarget (const char* name) const
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::RenderTarget");
}

/*
    Обновление
*/

void SceneRender::Update ()
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::Update");
}

/*
    Обмен
*/

void SceneRender::Swap (SceneRender& render)
{
  stl::swap (impl, render.impl);
}

namespace render
{

void swap (SceneRender& render1, SceneRender& render2)
{
  render1.Swap (render2);
}

}
