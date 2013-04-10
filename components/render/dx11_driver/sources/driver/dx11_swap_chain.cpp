#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

SwapChain::SwapChain (const DxFactoryPtr& factory, const DxDevicePtr& device, const SwapChainDesc& src_desc)
{
  try
  {
    if (!device)
      throw xtl::make_null_argument_exception ("", "device");

    if (!factory)
      throw xtl::make_null_argument_exception ("", "factory");

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

    check_errors ("IDXGIFactory::CreateSwapChain", factory->CreateSwapChain (&*device, &dx_desc, &dx_swap_chain));

    if (!dx_swap_chain)
      throw xtl::format_operation_exception ("", "IDXGIFactory::CreateSwapChain failed");

    swap_chain = DxSwapChainPtr (dx_swap_chain, false);
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
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Получение дескриптора
*/

void SwapChain::GetDesc (SwapChainDesc&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Получение устройства вывода с максимальным размером области перекрытия
*/

IOutput* SwapChain::GetContainingOutput ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Установка / взятие состояния full-screen mode
*/

void SwapChain::SetFullscreenState (bool state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

bool SwapChain::GetFullscreenState ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void SwapChain::Present ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Список свойств цепочки обмена
*/

IPropertyList* SwapChain::GetProperties ()
{
  return &properties;
}
