#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации рендера
*/

struct View::Impl
{
  RenderPtr      render;  //рендер
  render::Frame  frame;   //кадр
  Log            log;     //поток протоколирования

///Конструктор
  Impl (RenderManager& in_manager)
    : render (Render::GetRender (in_manager))
    , frame (in_manager.CreateFrame ())
  {
    log.Printf ("View created");
  }
  
///Деструктор
  ~Impl ()
  {
    log.Printf ("View destroyed");
  }
};

/*
    Конструктор / деструктор
*/

View::View (RenderManager& manager)
{
  try
  {
    impl = new Impl (manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::View::View");
    throw;
  }
}

View::~View ()
{
}

/*
    Кадр
*/

render::Frame& View::Frame ()
{
  return impl->frame;
}

/*
    Обновление камеры
*/

void View::UpdateCamera (scene_graph::Camera* camera)
{
  throw xtl::make_not_implemented_exception ("render:scene_graph3d::View::UpdateCamera");
}

/*
    Обновление свойств ренедринга
*/

void View::UpdateProperties (const common::PropertyMap&)
{
  throw xtl::make_not_implemented_exception ("render:scene_graph3d::View::UpdateProperties");
}

/*
    Обновление содержимого кадра
*/

void View::UpdateFrame ()
{
  throw xtl::make_not_implemented_exception ("render:scene_graph3d::View::UpdateFrame");
}

/*
    Подсчёт ссылок
*/

void View::AddRef ()
{
  addref (this);
}

void View::Release ()
{
  release (this);
}
