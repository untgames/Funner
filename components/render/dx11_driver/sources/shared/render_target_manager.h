#ifndef RENDER_DX11_DRIVER_RENDER_TARGET_MANAGER_HEADER
#define RENDER_DX11_DRIVER_RENDER_TARGET_MANAGER_HEADER

#include <stl/auto_ptr.h>

#include <render/low_level/device.h>

#include <shared/context_state.h>
#include <shared/device_manager.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetContext
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetContext  (const DxContextPtr& context, const DeviceManager& device_manager);
    ~RenderTargetContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IContextState& GetContextState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetRenderTargetView  (size_t render_target_slot, IView* render_target_view);
    void   SetDepthStencilView  (IView* depth_stencil_view);
    IView* GetRenderTargetView  (size_t render_target_slot) const;
    IView* GetDepthStencilView  () const;
    void   HasRenderTargetViews (bool states [DEVICE_RENDER_TARGET_SLOTS_COUNT]) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void            SetViewport (size_t render_target_slot, const Viewport& viewport);
    void            SetScissor  (size_t render_target_slot, const Rect& scissor_rect);
    const Viewport& GetViewport (size_t render_target_slot) const;
    const Rect&     GetScissor  (size_t render_target_slot) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ClearRenderTargetView (size_t render_target_slot, const Color4f& color);
    void ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil);
    void ClearViews            (size_t clear_flags, size_t views_count, const size_t* view_indices, const Color4f* colors, float depth, unsigned char stencil);

  private:
    RenderTargetContext (const RenderTargetContext&); //no impl
    RenderTargetContext& operator = (const RenderTargetContext&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetManager  (const DeviceManager&);
    ~RenderTargetManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetContext* CreateContext (const DxContextPtr& context);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IContextState* CreateContextState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture* CreateTexture             (const TextureDesc&);
    ITexture* CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index);
    ITexture* CreateDepthStencilTexture (ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IView* CreateView (ITexture* texture, const ViewDesc&);

  private:
    RenderTargetManager (const RenderTargetManager&); //no impl
    RenderTargetManager& operator = (const RenderTargetManager&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
