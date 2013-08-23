#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации цели рендеринга
*/

struct RenderTargetImpl::Impl
{
  stl::string          name;       //имя цели
  ConnectionPtr        connection; //соединение
  scene_graph::Screen* screen;     //экран  

/// Конструктор
  Impl (const ConnectionPtr& in_connection, const char* render_target_name)
    : connection (in_connection)
    , screen ()    
  {
    static const char* METHOD_NAME = "render::scene::client::RenderTargetImpl::RenderTargetImpl";

    if (!connection)
      throw xtl::make_null_argument_exception (METHOD_NAME, "connection");

    if (!render_target_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "render_target_name");

    name = render_target_name;
  }
};

/*
  Конструктор / деструктор
*/

RenderTargetImpl::RenderTargetImpl (const ConnectionPtr& connection, const char* render_target_name)
  : impl (new Impl (connection, render_target_name))
{
}

RenderTargetImpl::~RenderTargetImpl ()
{
}

/*
    Экран (политика владения - weak-ref)
*/

void RenderTargetImpl::SetScreen (scene_graph::Screen* screen)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

scene_graph::Screen* RenderTargetImpl::Screen ()
{
  return impl->screen;
}

/*
    Обновление
*/

void RenderTargetImpl::Update ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Захват изображения (screen-shot)
*/

void RenderTargetImpl::CaptureImage (media::Image&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
