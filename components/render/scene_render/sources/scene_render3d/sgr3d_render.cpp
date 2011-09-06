#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации рендера
*/

struct Render::Impl
{
  RenderManager& manager; //менеджер рендеринга
  render::Frame  frame;   //кадр
  Log            log;     //поток протоколирования
  
///Конструктор
  Impl (RenderManager& in_manager)
    : manager (in_manager)
    , frame (manager.CreateFrame ())
  {
    log.Printf ("SceneRender3D created");
  }
  
///Деструктор
  ~Impl ()
  {
    log.Printf ("SceneRender3D destroyed");
  }
};

/*
    Конструктор / деструктор
*/

Render::Render (RenderManager& manager)
{
  try
  {
    impl = new Impl (manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Render::Render");
    throw;
  }
}

Render::~Render ()
{
}

/*
    Кадр
*/

render::Frame& Render::Frame ()
{
  return impl->frame;
}

/*
    Обновление камеры
*/

void Render::UpdateCamera (scene_graph::Camera* camera)
{
  throw xtl::make_not_implemented_exception ("render::scene_graph3d::Render::UpdateCamera");
}

/*
    Обновление свойств ренедринга
*/

void Render::UpdateProperties (const common::PropertyMap&)
{
  throw xtl::make_not_implemented_exception ("render::scene_graph3d::Render::UpdateProperties");
}

/*
    Обновление содержимого кадра
*/

void Render::UpdateFrame ()
{
  throw xtl::make_not_implemented_exception ("render::scene_graph3d::Render::UpdateFrame");
}

/*
    Подсчёт ссылок
*/

void Render::AddRef ()
{
  addref (this);
}

void Render::Release ()
{
  release (this);
}
