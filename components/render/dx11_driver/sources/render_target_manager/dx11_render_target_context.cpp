#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    RenderTargetContextState
===================================================================================================
*/

typedef xtl::trackable_ptr<View> ViewPtr;

namespace
{

struct RenderTargetDesc
{
  ViewPtr  view;                      //текущее отображение буферов цвета
  Viewport viewport;                  //область вывода
  Rect     scissor;                   //область отсечения
  size_t   viewport_hash;             //хеш области вывода
  size_t   viewport_rect_hash;        //хэш области вывода для области отсечения
  size_t   scissor_hash;              //хэш области отсечения
  bool     need_recalc_viewport_hash; //флаг необходимости пересчёта хэша области вывода
  bool     need_recalc_scissor_hash;  //флаг необходимости пересчёта хэша области отсечения

/// Конструктор
  RenderTargetDesc () 
    : viewport_hash ()
    , viewport_rect_hash ()
    , scissor_hash ()
    , need_recalc_viewport_hash (true)
    , need_recalc_scissor_hash (true)
  {
    memset (&viewport, 0, sizeof viewport);
    memset (&scissor, 0, sizeof scissor);
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
    RenderTargetDesc& render_target = GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetContextState::Impl::GetViewportHash");

    if (render_target.need_recalc_viewport_hash)
    {
      render_target.viewport_hash             = common::crc32 (&render_target.viewport, sizeof render_target.viewport);
      render_target.viewport_rect_hash        = common::crc32 (&render_target.viewport, sizeof (Rect));
      render_target.need_recalc_viewport_hash = false;
    }

    return render_target.viewport_hash;
  }

  size_t GetViewportRectHash (size_t view_index)
  {
    GetViewportHash (view_index); //for update

    return render_targets [view_index].viewport_rect_hash;
  }

///Получение хэша области отсечения
  size_t GetScissorHash (size_t view_index)
  {
    RenderTargetDesc& render_target = GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetContextState::Impl::GetScissorHash");

    if (render_target.need_recalc_scissor_hash)
    {
      render_target.scissor_hash             = common::crc32 (&render_target.scissor, sizeof render_target.scissor);
      render_target.need_recalc_scissor_hash = false;
    }

    return render_target.scissor_hash;
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
  static const char* METHOD_NAME = "render::low_level::dx11::RenderTargetContextState::SetRenderTargetView";

  View* view = cast_object<View> (*impl, render_target_view, METHOD_NAME, "render_target_view");

  RenderTargetDesc& render_target = impl->GetRenderTarget (view_index, METHOD_NAME);

  if (render_target.view == view)
    return;

  if (view && view->GetType () != ViewType_RenderTarget)
    throw xtl::make_argument_exception (METHOD_NAME, "view", "<view>", "View type must have RenderTarget type");

  render_target.view = view;
  
  impl->UpdateNotify ();
}

void RenderTargetContextState::SetDepthStencilView (IView* depth_stencil_view)
{
  static const char* METHOD_NAME = "render::low_level::dx11::RenderTargetContextState::SetDepthStencilView";

  View* view = cast_object<View> (*impl, depth_stencil_view, METHOD_NAME, "depth_stencil_view");

  if (impl->depth_stencil_view == view)
    return;

  if (view && view->GetType () != ViewType_DepthStencil)
    throw xtl::make_argument_exception (METHOD_NAME, "view", "<view>", "View type must have DepthStencil type");

  impl->depth_stencil_view = view;
  
  impl->UpdateNotify ();  
}

IView* RenderTargetContextState::GetRenderTargetView (size_t view_index) const
{
  return &*impl->GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetContextState::GetRenderTargetView").view;
}

IView* RenderTargetContextState::GetDepthStencilView () const
{
  return &*impl->depth_stencil_view;
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

void RenderTargetContextState::CopyTo (const StateBlockMask&, RenderTargetContextState& dst_state) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
===================================================================================================
    RenderTargetContext
===================================================================================================
*/

struct RenderTargetContext::Impl: public RenderTargetContextState::Impl
{
  DxContextPtr context; //контекст

/// Конструктор
  Impl (const DeviceManager& device_manager, const DxContextPtr& in_context)
    : RenderTargetContextState::Impl (device_manager)
    , context (in_context)
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
    Флаг измененности состояния
*/

bool RenderTargetContext::IsDirty () const
{
  return GetImpl ().is_dirty;
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
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetContext::Bind");
    throw;
  }
}
