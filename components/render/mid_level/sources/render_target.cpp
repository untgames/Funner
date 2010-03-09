#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    Описание реализации цели рендеринга
*/

struct RenderTargetImpl::Impl
{
};

/*
    Конструктор / деструктор
*/

RenderTargetImpl::RenderTargetImpl ()
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderTargetImpl::RenderTargetImpl");
}

RenderTargetImpl::~RenderTargetImpl ()
{
}

/*
    Размеры цели рендеринга
*/

size_t RenderTargetImpl::Width ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderTargetImpl::Width");
}

size_t RenderTargetImpl::Height ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderTargetImpl::Height");
}

/*
    Захват изображения
*/

void RenderTargetImpl::Capture (media::Image& image)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderTargetImpl::Capture");
}
