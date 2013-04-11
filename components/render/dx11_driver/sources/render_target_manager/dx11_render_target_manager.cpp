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

ITexture* RenderTargetManager::CreateTexture (const TextureDesc& src_desc)
{
  try
  {
    D3D11_BUFFER_DESC dst_desc;

    memset (&dst_desc, 0, sizeof (dst_desc));

    dst_desc.ByteWidth = get_uncompressed_image_size (src_desc.width, src_desc.height, src_desc.format);
    dst_desc.Usage     = D3D11_USAGE_DEFAULT;

    struct FlagMap
    {
      BindFlag src_flag;
      UINT     dst_flag;
    };

    static const FlagMap flags [] = {
      {BindFlag_VertexBuffer,   D3D11_BIND_VERTEX_BUFFER},
      {BindFlag_IndexBuffer,    D3D11_BIND_INDEX_BUFFER},
      {BindFlag_ConstantBuffer, D3D11_BIND_CONSTANT_BUFFER},
      {BindFlag_Texture,        D3D11_BIND_SHADER_RESOURCE},
      {BindFlag_RenderTarget,   D3D11_BIND_RENDER_TARGET},
      {BindFlag_DepthStencil,   D3D11_BIND_DEPTH_STENCIL},
    };

    static size_t flags_count = sizeof (flags) / sizeof (*flags);

    dst_desc.BindFlags = 0;

    for (size_t i=0; i<flags_count; i++)
      if (src_desc.bind_flags & flags [i].src_flag)
        dst_desc.BindFlags |= flags [i].dst_flag;    

    dst_desc.CPUAccessFlags = 0;

    if (src_desc.access_flags & AccessFlag_Read)  dst_desc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
    if (src_desc.access_flags & AccessFlag_Write) dst_desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;

    dst_desc.MiscFlags           = 0;
    dst_desc.StructureByteStride = 0;

    ID3D11Buffer* buffer = 0;

    check_errors ("D3D11Device::CreateBuffer", impl->GetDeviceManager ().GetDevice ().CreateBuffer (&dst_desc, 0, &buffer));

    if (!buffer)
      throw xtl::format_operation_exception ("", "D3D11Device::CreateBuffer failed");

    return new RenderBuffer (impl->GetDeviceManager (), src_desc, DxResourcePtr (buffer, false));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RenderTargetManager::CreateTexture");
    throw;
  }
}

ITexture* RenderTargetManager::CreateRenderTargetTexture (ISwapChain* in_swap_chain, size_t buffer_index)
{
  try
  {
    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    ISwapChainImpl* swap_chain_impl = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");
    IDXGISwapChain& swap_chain      = swap_chain_impl->GetHandle ();   
    ID3D11Resource* resource        = 0;

    check_errors ("IDXGISwapChain::GetBuffer", swap_chain.GetBuffer (buffer_index, __uuidof (resource), reinterpret_cast<void**> (&resource)));

    if (!resource)
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

    return new RenderBuffer (impl->GetDeviceManager (), dst_desc, DxResourcePtr (resource, false)); //??? ref count
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
    dst_desc.access_flags         = AccessFlag_Read | AccessFlag_Write;
    dst_desc.bind_flags           = BindFlag_DepthStencil;
    dst_desc.usage_mode           = UsageMode_Static;

    return CreateTexture (dst_desc);
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

IView* RenderTargetManager::CreateView (ITexture* texture, const ViewDesc&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
