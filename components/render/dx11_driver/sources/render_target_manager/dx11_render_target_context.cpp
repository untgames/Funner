#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    RenderTargetContextState
===================================================================================================
*/

typedef xtl::com_ptr<View> ViewPtr;

namespace
{

struct RenderTargetDesc
{
  ViewPtr         view;                      //текущее отображение буферов цвета
  Viewport        viewport;                  //область вывода
  Rect            scissor;                   //область отсечения
  mutable size_t  viewport_hash;             //хеш области вывода
  mutable size_t  scissor_hash;              //хэш области отсечения
  mutable bool    need_recalc_viewport_hash; //флаг необходимости пересчёта хэша области вывода
  mutable bool    need_recalc_scissor_hash;  //флаг необходимости пересчёта хэша области отсечения

/// Конструктор
  RenderTargetDesc () 
    : viewport_hash ()
    , scissor_hash ()
    , need_recalc_viewport_hash (true)
    , need_recalc_scissor_hash (true)
  {
    memset (&viewport, 0, sizeof viewport);
    memset (&scissor, 0, sizeof scissor);
  }

///Получение хэша области вывода
  size_t GetViewportHash () const
  {
    if (need_recalc_viewport_hash)
    {
      viewport_hash             = common::crc32 (&viewport, sizeof viewport);
      need_recalc_viewport_hash = false;
    }

    return viewport_hash;
  }

///Получение хэша области отсечения
  size_t GetScissorHash () const
  {
    if (need_recalc_scissor_hash)
    {
      scissor_hash             = common::crc32 (&scissor, sizeof scissor);
      need_recalc_scissor_hash = false;
    }

    return scissor_hash;
  }    
};

}

struct RenderTargetContextState::Impl: public DeviceObject
{
  RenderTargetDesc render_targets [DEVICE_RENDER_TARGET_SLOTS_COUNT]; //дескрипторы целей рендеринга
  ViewPtr          depth_stencil_view;                                //текущее отображение буфера попиксельного отсечения
  bool             is_dirty;                                          //флаг "грязности"

/// Конструктор
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
    , is_dirty (true)
  {
  }

/// Десктруктор
  virtual ~Impl () {}

/// Получение цели
  RenderTargetDesc& GetRenderTarget (size_t view_index, const char* source)
  {
    if (view_index >= DEVICE_RENDER_TARGET_SLOTS_COUNT)
      throw xtl::make_range_exception (source, "render_target_slot", view_index, DEVICE_RENDER_TARGET_SLOTS_COUNT);

    return render_targets [view_index];
  }

/// Оповещение об изменении состояния
  void UpdateNotify ()
  {
    is_dirty = true;
  }

///Получение хэша области вывода
  size_t GetViewportHash (size_t view_index)
  {
    return GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetContextState::Impl::GetViewportHash").GetViewportHash ();
  }

///Получение хэша области отсечения
  size_t GetScissorHash (size_t view_index)
  {
    return GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetContextState::Impl::GetScissorHash").GetScissorHash ();
  }    
};

/*
    Конструктор / деструктор
*/

RenderTargetContextState::RenderTargetContextState (const DeviceManager& manager)
  : impl (new Impl (manager))
{
}

RenderTargetContextState::RenderTargetContextState (Impl* in_impl)
  : impl (in_impl)
{
}

RenderTargetContextState::~RenderTargetContextState ()
{
}

/*
    Получение реализации
*/

RenderTargetContextState::Impl& RenderTargetContextState::GetImpl () const
{
  return *impl;
}

/*
    Выбор целевых отображений
*/

void RenderTargetContextState::SetRenderTargetView (size_t view_index, IView* render_target_view)
{
  try
  {
    View* view = cast_object<View> (*impl, render_target_view, "", "render_target_view");

    RenderTargetDesc& render_target = impl->GetRenderTarget (view_index, "");

    if (render_target.view == view)
      return;

    if (view && view->GetType () != ViewType_RenderTarget)
      throw xtl::make_argument_exception ("", "view", "<view>", "View type must have RenderTarget type");

    render_target.view = view;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetContextState::SetRenderTargetView");
    throw;
  }
}

void RenderTargetContextState::SetDepthStencilView (IView* depth_stencil_view)
{
  try
  {
    View* view = cast_object<View> (*impl, depth_stencil_view, "", "depth_stencil_view");

    if (impl->depth_stencil_view == view)
      return;

    if (view && view->GetType () != ViewType_DepthStencil)
      throw xtl::make_argument_exception ("", "view", "<view>", "View type must have DepthStencil type");

    impl->depth_stencil_view = view;
    
    impl->UpdateNotify ();  
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetContextState::SetDepthStencilView");
    throw;
  }
}

IView* RenderTargetContextState::GetRenderTargetView (size_t view_index) const
{
  return impl->GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetContextState::GetRenderTargetView").view.get ();
}

IView* RenderTargetContextState::GetDepthStencilView () const
{
  return impl->depth_stencil_view.get ();
}

void RenderTargetContextState::HasRenderTargetViews (bool states [DEVICE_RENDER_TARGET_SLOTS_COUNT]) const
{  
  for (size_t i=0; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
    states [i] = impl->render_targets [i].view.get () != 0;
}

/*
    Настройка подуровня растеризации
*/

void RenderTargetContextState::SetViewport (size_t view_index, const Viewport& viewport)
{
  RenderTargetDesc& render_target = impl->GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetManagerState::SetViewport");

  render_target.viewport                  = viewport;
  render_target.need_recalc_viewport_hash = true;

  impl->UpdateNotify ();
}

void RenderTargetContextState::SetScissor (size_t view_index, const Rect& scissor_rect)
{
  RenderTargetDesc& render_target = impl->GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetManagerState::SetScissor");

  render_target.scissor                  = scissor_rect;
  render_target.need_recalc_scissor_hash = true;

  impl->UpdateNotify ();
}

const Viewport& RenderTargetContextState::GetViewport (size_t view_index) const
{
  return impl->GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetManagerState::GetViewport").viewport;
}

const Rect& RenderTargetContextState::GetScissor (size_t view_index) const
{
  return impl->GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetManagerState::GetScissor").scissor;
}

/*
    Захват / восстановление состояния
*/

void RenderTargetContextState::CopyTo (const StateBlockMask& mask, RenderTargetContextState& dst_state) const
{
  try
  {
    if (!mask.os_render_target_views && !mask.rs_viewports && !mask.rs_scissors)
      return;

    bool update_notify = false;

    for (size_t i=0; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
    {
      const RenderTargetDesc& src_desc = impl->render_targets [i];
      RenderTargetDesc&       dst_desc = dst_state.impl->render_targets [i];

      if (mask.os_render_target_views && src_desc.view != dst_desc.view)
      {
        dst_desc.view = src_desc.view;
        update_notify = true;
      }

      if (mask.rs_viewports && src_desc.GetViewportHash () != dst_desc.GetViewportHash ())
      {
        dst_desc.viewport                  = src_desc.viewport;
        dst_desc.need_recalc_viewport_hash = true;
        update_notify                      = true;
      }

      if (mask.rs_scissors && src_desc.GetScissorHash () != dst_desc.GetScissorHash ())
      {
        dst_desc.scissor                  = src_desc.scissor;
        dst_desc.need_recalc_scissor_hash = true;
        update_notify                     = true;
      }
    }

    if (mask.os_depth_stencil_view && impl->depth_stencil_view != dst_state.impl->depth_stencil_view)
    {
      dst_state.impl->depth_stencil_view = impl->depth_stencil_view;
      update_notify                      = true;
    }

    if (update_notify)
      dst_state.impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetContextState::CopyTo");
    throw;
  }
}

/*
===================================================================================================
    RenderTargetContext
===================================================================================================
*/

namespace
{

/// Кэш цели рендеринга
struct RenderTargetCache
{
  ID3D11RenderTargetView* view;             //отображение
  size_t                  viewport_hash;    //хэш области вывода
  size_t                  scissor_hash;     //хэш области отсечения

  RenderTargetCache ()
    : view ()
    , viewport_hash ()
    , scissor_hash ()
  {
  }
};

}

struct RenderTargetContext::Impl: public RenderTargetContextState::Impl
{
  DxContextPtr            context;                                                 //контекст
  RenderTargetCache       render_target_caches [DEVICE_RENDER_TARGET_SLOTS_COUNT]; //кэши целей рендеринга
  ID3D11DepthStencilView* depth_stencil_view_cache;                                //кэш отображения depth/stencil

/// Конструктор
  Impl (const DeviceManager& device_manager, const DxContextPtr& in_context)
    : RenderTargetContextState::Impl (device_manager)
    , context (in_context)
    , depth_stencil_view_cache ()
  {
    if (!context)
      throw xtl::make_null_argument_exception ("render::low_level::dx11::RenderTargetContext::Impl::Impl", "context");
  }
};

/*
    Конструктор / деструктор
*/

RenderTargetContext::RenderTargetContext (const DeviceManager& device_manager, const DxContextPtr& context)
  : RenderTargetContextState (new Impl (device_manager, context))
{
}

RenderTargetContext::~RenderTargetContext ()
{
}

/*
    Получение реализации
*/

RenderTargetContext::Impl& RenderTargetContext::GetImpl () const
{
  return static_cast<Impl&> (RenderTargetContextState::GetImpl ());
}

/*
    Очистка буферов отрисовки
*/

void RenderTargetContext::ClearRenderTargetView (size_t render_target_slot, const Color4f& color)
{
  try
  {
    ClearViews (ClearFlag_RenderTarget, 1, &render_target_slot, &color, 0.0f, 0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetContext::ClearRenderTargetView");
    throw;
  }
}

void RenderTargetContext::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  try
  {
    size_t  view_index = 0;
    Color4f clear_color;

    ClearViews (ClearFlag_Depth | ClearFlag_Stencil, 0, &view_index, &clear_color, depth, stencil);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetContext::ClearDepthStencilView");
    throw;
  }
}

void RenderTargetContext::ClearViews (size_t clear_flags, size_t views_count, const size_t* view_indices, const Color4f* colors, float depth, unsigned char stencil)
{
  try
  {
      //установка целей рендеринга и активного контекста

    Bind ();

      //очистка целей рендеринга

    Impl&                impl    = GetImpl ();
    ID3D11DeviceContext& context = *impl.context;

    if (clear_flags & ClearFlag_RenderTarget)
    {
        //проверка корректности аргументов

      if (views_count && !view_indices)
        throw xtl::make_null_argument_exception ("", "render_target_indices");

      if (views_count && !colors)
        throw xtl::make_null_argument_exception ("", "colors");

      for (size_t i=0; i<views_count; i++)
      {
        size_t view_index = view_indices [i];

        if (view_index >= DEVICE_RENDER_TARGET_SLOTS_COUNT)
          throw xtl::format_exception<xtl::bad_range> ("", "Argument <view_indices[%u]>=%u is out of range [%u;%u)", i, view_index, 0, DEVICE_RENDER_TARGET_SLOTS_COUNT);
      }

        //очистка целей рендеринга

      for (size_t i=0; i<views_count; i++)
      {
        size_t view_index = view_indices [i];

        RenderTargetDesc& render_target = impl.render_targets [view_index];
        const Color4f&    color         = colors [view_index];

        if (!render_target.view)
          continue;

        ID3D11RenderTargetView* view = static_cast<ID3D11RenderTargetView*> (&render_target.view->GetHandle ());

        context.ClearRenderTargetView (view, &color.red);
      }
    }

      //очистка depth/stencil

    if (impl.depth_stencil_view && (clear_flags & (ClearFlag_Depth | ClearFlag_Stencil)))
    {
      UINT flags = 0;

      if (clear_flags & ClearFlag_Depth)   flags |= D3D11_CLEAR_DEPTH;
      if (clear_flags & ClearFlag_Stencil) flags |= D3D11_CLEAR_STENCIL;

      ID3D11DepthStencilView* view = static_cast<ID3D11DepthStencilView*> (&impl.depth_stencil_view->GetHandle ());      

      context.ClearDepthStencilView (view, flags, depth, stencil);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetContext::ClearViews");
    throw;
  }
}

/*
    Биндинг
*/

void RenderTargetContext::Bind ()
{
  try
  {
    Impl& impl = GetImpl ();

    if (!impl.is_dirty)
      return;

      //очистка целей рендеринга

    ID3D11DeviceContext& context = *impl.context;

      //преобразование целей

    bool need_update_render_targets = false, need_update_viewports = false, need_update_scissors = false;

    ID3D11RenderTargetView* views [DEVICE_RENDER_TARGET_SLOTS_COUNT];
    D3D11_VIEWPORT          viewports [DEVICE_RENDER_TARGET_SLOTS_COUNT];
    D3D11_RECT              scissors [DEVICE_RENDER_TARGET_SLOTS_COUNT];
    size_t                  viewport_hashes [DEVICE_RENDER_TARGET_SLOTS_COUNT];
    size_t                  scissor_hashes [DEVICE_RENDER_TARGET_SLOTS_COUNT];

    memset (views, 0, sizeof (views));

    for (size_t i=0; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
    {
      const RenderTargetDesc&  render_target       = impl.render_targets [i];
      const RenderTargetCache& render_target_cache = impl.render_target_caches [i];
      ID3D11View*              view                = render_target.view ? &render_target.view->GetHandle () : (ID3D11View*)0;

      if (view != render_target_cache.view)
        need_update_render_targets = true;

      viewport_hashes [i] = impl.GetViewportHash (i);
      scissor_hashes [i]  = impl.GetScissorHash (i);

      if (viewport_hashes [i] != render_target_cache.viewport_hash)
        need_update_viewports = true;

      if (scissor_hashes [i] != render_target_cache.scissor_hash)
        need_update_scissors = true;

      views [i] = static_cast<ID3D11RenderTargetView*> (view);

      const Viewport& src_vp = render_target.viewport;
      D3D11_VIEWPORT& dst_vp = viewports [i];

      dst_vp.TopLeftX = (float)src_vp.x;
      dst_vp.TopLeftY = (float)src_vp.y;
      dst_vp.Width    = (float)src_vp.width;
      dst_vp.Height   = (float)src_vp.height;
      dst_vp.MinDepth = src_vp.min_depth;
      dst_vp.MaxDepth = src_vp.max_depth;

      const Rect& src_scissor = render_target.scissor;
      D3D11_RECT& dst_scissor = scissors [i];

      dst_scissor.left   = src_scissor.x;
      dst_scissor.top    = src_scissor.y;
      dst_scissor.right  = src_scissor.x + src_scissor.width;
      dst_scissor.bottom = src_scissor.y + src_scissor.height;
    }

      //установка в контекст

    ID3D11DepthStencilView* depth_stencil_view = impl.depth_stencil_view ? static_cast<ID3D11DepthStencilView*> (&impl.depth_stencil_view->GetHandle ()) : (ID3D11DepthStencilView*)0;

    if (need_update_render_targets || depth_stencil_view != impl.depth_stencil_view_cache)
      context.OMSetRenderTargets (DEVICE_RENDER_TARGET_SLOTS_COUNT, views, depth_stencil_view);

    if (need_update_viewports)
      context.RSSetViewports (DEVICE_RENDER_TARGET_SLOTS_COUNT, viewports);

    if (need_update_scissors)
      context.RSSetScissorRects (DEVICE_RENDER_TARGET_SLOTS_COUNT, scissors);

      //обновление кэша

    for (size_t i=0; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
    {
      RenderTargetCache& render_target_cache = impl.render_target_caches [i];

      render_target_cache.view          = views [i];
      render_target_cache.viewport_hash = viewport_hashes [i];
      render_target_cache.scissor_hash  = scissor_hashes [i];
    }

    impl.depth_stencil_view_cache = static_cast<ID3D11DepthStencilView*> (depth_stencil_view);

      //очистка флага грязности состояния

    impl.is_dirty = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetContext::Bind");
    throw;
  }
}
