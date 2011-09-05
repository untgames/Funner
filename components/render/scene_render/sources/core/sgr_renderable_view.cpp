#include "shared.h"

using namespace render;

/*
    Описание реализации области отрисовки
*/

struct RenderableView::Impl: public scene_graph::IViewportListener, public scene_graph::IScreenListener
{
  RenderManager&         manager;  //менеджер рендеринга
  scene_graph::Screen&   screen;   //экран
  scene_graph::Viewport& viewport; //область вывода
  ISceneRenderPtr        renderer; //рендер сцены  
  
///Конструктор
  Impl (RenderManager& in_manager, scene_graph::Screen& in_screen, scene_graph::Viewport& in_viewport)
    : manager (in_manager)
    , screen (in_screen)
    , viewport (in_viewport)
    , renderer (SceneRenderManagerSingleton::Instance ()->CreateRender (manager, viewport.Renderer ()))
  {    
  }

///Область экрана обновлена
  void OnScreenChangeArea (const Rect& new_area)
  {
  }

///Область вывода обновлена
  void OnViewportChangeArea (const Rect& new_area)
  {
  }

///Изменена камера
  void OnViewportChangeCamera (scene_graph::Camera* new_camera)
  {
  }

///Изменен порядок областей вывода
  void OnViewportChangeZOrder (int new_z_order)
  {
  }

///Изменена активность области вывода
  void OnViewportChangeActive (bool new_state)
  {
  }
  
///Изменены параметры очистки области вывода
  void OnViewportChangeBackground (bool new_state, const math::vec4f& new_color)
  {
  }

///Изменен путь рендеринга
  void OnViewportChangeRenderer (const char* new_renderer)
  {
  }

///Изменены свойства рендеринга
  void OnViewportChangeProperties (const common::PropertyMap& properties)
  {
  }
};

/*
    Конструктор / деструктор
*/

RenderableView::RenderableView (RenderManager& manager, scene_graph::Screen& screen, scene_graph::Viewport& viewport)
{
  try
  {
    impl = new Impl (manager, screen, viewport);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderableView::RenderableView");
    throw;
  }
}

RenderableView::~RenderableView ()
{
}

/*
    Область вывода / экран
*/

const scene_graph::Viewport& RenderableView::Viewport ()
{
  return impl->viewport;
}

const scene_graph::Screen& RenderableView::Screen ()
{
  return impl->screen;
}

/*
    Регистрация целевых буферов отрисовки
*/

void RenderableView::SetRenderTarget (const char* name, const RenderTarget& target)
{
  throw xtl::make_not_implemented_exception ("render::RenderableView::SetRenderTarget");
}

void RenderableView::RemoveRenderTarget (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::RenderableView::RemoveRenderTarget");
}

/*
    Обновление кадра
*/

void RenderableView::UpdateFrame (Frame* parent_frame)
{
  throw xtl::make_not_implemented_exception ("render::RenderableView::Update");
}
