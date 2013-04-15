#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Описание реализации менеджера целей рендеринга
*/

struct RenderTargetManager::Impl: public DeviceObject
{
/// Конструктор
  Impl (const DeviceManager& device_manager)
    : DeviceObject (device_manager)
  {
  }
};

/*
    Конструктор / деструктор
*/

RenderTargetManager::RenderTargetManager (const DeviceManager& device_manager)
  : impl (new Impl (device_manager))
{
}

RenderTargetManager::~RenderTargetManager ()
{
}

/*
    Создание текстур
*/

ITexture* RenderTargetManager::CreateRenderTargetTexture (ISwapChain* in_swap_chain, size_t buffer_index)
{
  try
  {
    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    ISwapChainImpl*  swap_chain_impl = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");
    IDXGISwapChain&  swap_chain      = swap_chain_impl->GetHandle ();   
    ID3D11Texture2D* texture         = 0;

    check_errors ("IDXGISwapChain::GetBuffer", swap_chain.GetBuffer (buffer_index, __uuidof (texture), reinterpret_cast<void**> (&texture)));

    if (!texture)
      throw xtl::format_operation_exception ("", "IDXGISwapChain::GetBuffer failed");

    DXGI_SWAP_CHAIN_DESC src_desc;

    memset (&src_desc, 0, sizeof (src_desc));

    check_errors ("IDXGISwapChain::GetDesc", swap_chain.GetDesc (&src_desc));

    TextureDesc dst_desc;

    memset (&dst_desc, 0, sizeof (dst_desc));

    dst_desc.dimension            = TextureDimension_2D;
    dst_desc.width                = src_desc.BufferDesc.Width;
    dst_desc.height               = src_desc.BufferDesc.Height;
    dst_desc.layers               = 1;
    dst_desc.format               = PixelFormat_RGBA8;
    dst_desc.generate_mips_enable = false;
    dst_desc.access_flags         = AccessFlag_Read | AccessFlag_Write;
    dst_desc.bind_flags           = BindFlag_RenderTarget;
    dst_desc.usage_mode           = UsageMode_Static;

//    return new RenderBuffer (impl->GetDeviceManager (), dst_desc, DxTexture2DPtr (texture, false)); //??? ref count
    return new Texture (impl->GetDeviceManager (), dst_desc, DxResourcePtr (texture));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetManager::CreateRenderTargetTexture");
    throw;
  }
}

ITexture* RenderTargetManager::CreateDepthStencilTexture (ISwapChain* in_swap_chain)
{
  try
  {
    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    ISwapChainImpl* swap_chain_impl = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");
    IDXGISwapChain& swap_chain      = swap_chain_impl->GetHandle ();   

    DXGI_SWAP_CHAIN_DESC src_desc;

    memset (&src_desc, 0, sizeof (src_desc));

    check_errors ("IDXGISwapChain::GetDesc", swap_chain.GetDesc (&src_desc));

    TextureDesc dst_desc;

    memset (&dst_desc, 0, sizeof (dst_desc));

    dst_desc.dimension            = TextureDimension_2D;
    dst_desc.width                = src_desc.BufferDesc.Width;
    dst_desc.height               = src_desc.BufferDesc.Height;
    dst_desc.layers               = 1;
    dst_desc.format               = PixelFormat_D24S8;
    dst_desc.generate_mips_enable = false;
    dst_desc.access_flags         = 0;
    dst_desc.bind_flags           = BindFlag_DepthStencil;
    dst_desc.usage_mode           = UsageMode_Default;

    return new Texture (impl->GetDeviceManager (), dst_desc, 0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetManager::CreateDepthStencilTexture");
    throw;
  }
}

/*
    Создание отображений
*/

IView* RenderTargetManager::CreateView (ITexture* texture, const ViewDesc& desc)
{
  try
  {
    return new View (impl->GetDeviceManager (), texture, desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetManager::CreateView");
    throw;
  }
}
