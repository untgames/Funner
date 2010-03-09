#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    Описание реализации окна рендеринга
*/

struct WindowImpl::Impl
{
};

/*
    Конструктор / деструктор
*/

WindowImpl::WindowImpl (syslib::Window& window)
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::WindowImpl::WindowImpl");
}

WindowImpl::~WindowImpl ()
{
}

/*
    Идентификатор окна
*/

void WindowImpl::SetId (const char* name)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::WindowImpl::SetId");
}

const char* WindowImpl::Id ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::WindowImpl::Id");
}

/*
    Цели рендеринга
*/

const RenderTargetPtr& WindowImpl::ColorBuffer ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::WindowImpl::ColorBuffer");
}

const RenderTargetPtr& WindowImpl::DepthStencilBuffer ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::WindowImpl::DepthStencilBuffer");
}

/*
    Размеры целей рендеринга
*/

size_t WindowImpl::Width ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::WindowImpl::Width");
}

size_t WindowImpl::Height ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::WindowImpl::Height");
}

/*
    Регистрация обработчиков событий
*/

xtl::connection WindowImpl::RegisterEventHandler (WindowEvent event, const EventHandler& handler) const
{
  throw xtl::make_not_implemented_exception ("render::mid_level::WindowImpl::RegisterEventHandler");
}
