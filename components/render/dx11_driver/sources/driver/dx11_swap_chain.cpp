#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

SwapChain::SwapChain (const DxFactoryPtr& factory, const AdapterPtr& in_adapter, const DxDevicePtr& device, const SwapChainDesc& src_desc)
{
  try
  {
    if (!device)
      throw xtl::make_null_argument_exception ("", "device");

    if (!in_adapter)
      throw xtl::make_null_argument_exception ("", "adapter");

    if (!factory)
      throw xtl::make_null_argument_exception ("", "factory");

    adapter = in_adapter;

    DXGI_SWAP_CHAIN_DESC dx_desc;

    memset (&dx_desc, 0, sizeof (dx_desc));

    dx_desc.BufferDesc.Width                   = src_desc.frame_buffer.width;
    dx_desc.BufferDesc.Height                  = src_desc.frame_buffer.height;
    dx_desc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    dx_desc.BufferDesc.RefreshRate.Numerator   = 60;
    dx_desc.BufferDesc.RefreshRate.Denominator = 1;   
    dx_desc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
    dx_desc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; 
    dx_desc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;// | DXGI_USAGE_UNORDERED_ACCESS;
    dx_desc.BufferCount                        = src_desc.buffers_count;
    dx_desc.OutputWindow                       = (HWND)src_desc.window_handle;
    dx_desc.SampleDesc.Count                   = src_desc.samples_count;
    dx_desc.SampleDesc.Quality                 = 0;
    dx_desc.Windowed                           = !src_desc.fullscreen;

    switch (src_desc.swap_method)
    {
      case SwapMethod_Discard:
        dx_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        break;
      default:
      case SwapMethod_Flip:
      case SwapMethod_Copy:
        dx_desc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
        break;
    }

//TODO: MakeWindowAssociation

    IDXGISwapChain* dx_swap_chain = 0;

    check_errors ("IDXGIFactory::CreateSwapChain", factory->CreateSwapChain (device.get (), &dx_desc, &dx_swap_chain));

    if (!dx_swap_chain)
      throw xtl::format_operation_exception ("", "IDXGIFactory::CreateSwapChain failed");

    swap_chain = DxSwapChainPtr (dx_swap_chain, false);

    check_errors ("IDXGISwapChain::GetDesc", swap_chain->GetDesc (&dx_desc));

    memset (&desc, 0, sizeof (desc));

    desc = src_desc;

    desc.frame_buffer.width        = dx_desc.BufferDesc.Width;
    desc.frame_buffer.height       = dx_desc.BufferDesc.Height;
    desc.frame_buffer.color_bits   = 32;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = 24;
    desc.frame_buffer.stencil_bits = 8;
    desc.samples_count             = dx_desc.SampleDesc.Count;
    desc.buffers_count             = dx_desc.BufferCount;
    desc.fullscreen                = !dx_desc.Windowed; 

    switch (dx_desc.SwapEffect)
    {
      case DXGI_SWAP_EFFECT_DISCARD:
        desc.swap_method = SwapMethod_Discard;
        break;
      default:
      case DXGI_SWAP_EFFECT_SEQUENTIAL:
        desc.swap_method = SwapMethod_Copy;
        break;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SwapChain::SwapChain");
    throw;
  }
}

SwapChain::~SwapChain ()
{
}

/*
    Получение адаптера
*/

IAdapter* SwapChain::GetAdapter ()
{
  return adapter.get ();
}

/*
    Низкоуровневый дескриптор
*/

IDXGISwapChain& SwapChain::GetHandle ()
{
  return *swap_chain;
}

/*
    Получение дескриптора
*/

void SwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = desc;
}

/*
    Получение устройства вывода с максимальным размером области перекрытия
*/

IOutput* SwapChain::GetContainingOutput ()
{
  try
  {
    IDXGIOutput* output = 0;

    check_errors ("IDXGISwapChain::GetContainingOutput", swap_chain->GetContainingOutput (&output));

    if (!output)
      throw xtl::format_operation_exception ("", "IDXGISwapChain::GetContainingOutput failed");

    return adapter->FindOutput (output);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SwapChain::GetContainingOutput");
    throw;
  }
}

/*
    Установка / взятие состояния full-screen mode
*/

void SwapChain::SetFullscreenState (bool state)
{
  try
  {
    IDXGIOutput* output = 0;

    check_errors ("IDXGISwapChain::GetContainingOutput", swap_chain->GetContainingOutput (&output));

    if (!output)
      throw xtl::format_operation_exception ("", "IDXGISwapChain::GetContainingOutput failed");

    check_errors ("IDXGISwapChain::SetFullscreenState", swap_chain->SetFullscreenState (state, output));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SwapChain::SetFullscreenState");
    throw;
  }
}

bool SwapChain::GetFullscreenState ()
{
  try
  {
    BOOL state = FALSE;

    check_errors ("IDXGISwapChain::GetFullscreenState", swap_chain->GetFullscreenState (&state, 0));

    return state != FALSE;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SwapChain::GetFullscreenState");
    throw;
  }
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void SwapChain::Present ()
{
  try
  {
    check_errors ("IDXGISwapChain::Present", swap_chain->Present (desc.vsync, 0));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SwapChain::Present");
    throw;
  }
}

/*
    Список свойств цепочки обмена
*/

IPropertyList* SwapChain::GetProperties ()
{
  return &properties;
}
