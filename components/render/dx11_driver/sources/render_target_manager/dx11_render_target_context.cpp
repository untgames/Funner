#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Описание реализации контекста целей рендеринга
*/

struct RenderTargetContext::Impl: public DeviceObject
{
  DxContextPtr context; //ссылка на контекст

/// Конструктор
  Impl (const DxContextPtr& in_context, const DeviceManager& device_manager)
    : DeviceObject (device_manager)
    , context (in_context)
  {
    if (!context)
      throw xtl::make_null_argument_exception ("render::low_level::dx11::RenderTargetContext::Impl::Impl", "context");
  }
};

/*
    Конструктор / деструктор
*/

RenderTargetContext::RenderTargetContext (const DxContextPtr& context, const DeviceManager& device_manager)
  : impl (new Impl (context, device_manager))
{
}

RenderTargetContext::~RenderTargetContext ()
{
}

/*
    Очистка буферов отрисовки
*/

void RenderTargetContext::ClearRenderTargetView (size_t render_target_slot, const Color4f& color)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderTargetContext::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderTargetContext::ClearViews (size_t clear_flags, size_t views_count, const size_t* view_indices, const Color4f* colors, float depth, unsigned char stencil)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Биндинг
*/

void RenderTargetContext::Bind ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
