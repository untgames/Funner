#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::windows;

namespace
{

//проверка корректности оконного дескриптора
HWND check_output_window (const SwapChainDesc& desc)
{
  HWND window = (HWND)desc.window_handle;
  
  if (!window)
    throw xtl::make_null_argument_exception ("render::low_level::opengl::windows::PrimarySwapChain::PrimarySwapChain", "swap_chain_desc.window_handle");
    
  return window;
}

}

/*
    Описание реализации PrimarySwapChain
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef xtl::signal<void ()>  ChangeDisplayModeSignal;

struct PrimarySwapChain::Impl: private IWindowListener
{
  AdapterPtr              adapter;                //адаптер
  WglExtensionEntries     wgl_extension_entries;  //таблица WGL-расширений
  HWND                    output_window;          //окно вывода
  HDC                     output_context;         //контекст вывода
  DummyWindow             listener_window;        //следящее окно
  SwapChainDesc           desc;                   //дескриптор цепочки обмена
  ChangeDisplayModeSignal cdm_signal;             //сигнал, оповещающий об изменении видео-режима
  PropertyList            properties;             //свойства цепочки обмена

///Конструктор
  Impl (const SwapChainDesc& in_desc, const PixelFormatDesc& pixel_format)
    : adapter (pixel_format.adapter),
      output_window (check_output_window (in_desc)),
      output_context (::GetDC (output_window)),
      listener_window (output_window, this)
  {
      //проверка корректности операции

    if (!output_context)
      raise_error ("GetDC");

      //инициализация таблицы расширений

    if (pixel_format.wgl_extension_entries)
    {
      wgl_extension_entries = *pixel_format.wgl_extension_entries;
    }
    else
    {
      memset (&wgl_extension_entries, 0, sizeof wgl_extension_entries);
    }    

      //установка состояния FullScreen

    if (in_desc.fullscreen)
      SetFullscreenState (true, in_desc.frame_buffer.color_bits);

      //установка свойств цепочки обмена

    properties.AddProperty ("gl_extensions", get_wgl_extensions_string (wgl_extension_entries, output_context).c_str ());

      //установка текущего формата пикселей для контекста отрисовки

    adapter->SetPixelFormat (output_context, pixel_format.pixel_format_index);

      //инициализация дескриптора цепочки обмена      

    desc.frame_buffer.width        = GetDeviceCaps (output_context, HORZRES);
    desc.frame_buffer.height       = GetDeviceCaps (output_context, VERTRES);
    desc.frame_buffer.color_bits   = pixel_format.color_bits;
    desc.frame_buffer.alpha_bits   = pixel_format.alpha_bits;
    desc.frame_buffer.depth_bits   = pixel_format.depth_bits;
    desc.frame_buffer.stencil_bits = pixel_format.stencil_bits;
    desc.samples_count             = pixel_format.samples_count;
    desc.buffers_count             = pixel_format.buffers_count;
    desc.swap_method               = pixel_format.swap_method;    
    desc.fullscreen                = GetFullscreenState ();
    desc.vsync                     = in_desc.vsync;
    desc.window_handle             = in_desc.window_handle;    
  }    
  
///Деструктор
  ~Impl ()
  {
    ::ReleaseDC (output_window, output_context);
  }  
  
///Установка состояния полноэкранного режима
  void SetFullscreenState (bool state, size_t color_bits)
  {
    if (GetFullscreenState () == state)
      return;

    Output* output = adapter->FindContainingOutput (output_window);
    
    if (!output)
      return;

    if (state)
    {
      if (!SetWindowPos (output_window, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE))
        raise_error ("SetWindowPos");

      OutputModeDesc mode_desc;

      RECT window_rect;

      if (!GetWindowRect (output_window, &window_rect))
        raise_error ("GetWindowRect");

      mode_desc.width        = window_rect.right - window_rect.left;
      mode_desc.height       = window_rect.bottom - window_rect.top;
      mode_desc.color_bits   = color_bits;
      mode_desc.refresh_rate = 0;    

      output->SetCurrentMode (mode_desc);
    }
    else
    {
      output->RestoreDefaultMode ();
    }
  }

///Получение состояния полноэкранного режима
  bool GetFullscreenState ()
  {
    Output* output = adapter->FindContainingOutput (output_window);

    if (!output)
      return false;

    OutputModeDesc mode_desc;
    
    output->GetCurrentMode (mode_desc);

    RECT window_rect;

    if (!GetWindowRect (output_window, &window_rect))
      raise_error ("GetWindowRect");

    return window_rect.left == 0 && window_rect.top == 0 && window_rect.right - window_rect.left == mode_desc.width &&
           window_rect.bottom - window_rect.top == mode_desc.height && desc.frame_buffer.color_bits >= mode_desc.color_bits;
  }  
  
///Обработка события удаления окна
  void OnDestroy ()
  {
      //если текущий контекст вывода является контекстом вывода окна - сбрасываем текущий контекст

    if (adapter->GetCurrentDC () == output_context)
      adapter->MakeCurrent (0, 0);
  }

///Обработка события изменения видео-режима
  void OnDisplayModeChange ()
  {
    try
    {
        //оповещение об изменении виедо-режима

      cdm_signal ();
    }
    catch (...)
    {
      //подавление всех исключений
    }    
  }
};

/*
    Конструктор / деструктор
*/

PrimarySwapChain::PrimarySwapChain (const SwapChainDesc& desc, const PixelFormatDesc& pixel_format)
  : impl (new Impl (desc, pixel_format))
{
}

PrimarySwapChain::~PrimarySwapChain ()
{ 
  try
  {
    if (GetFullscreenState () && impl->desc.fullscreen)
      SetFullscreenState (false);
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Получение адаптера
*/

Adapter* PrimarySwapChain::GetAdapterImpl ()
{
  return impl->adapter.get ();
}

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    Реалиация интерфейса IDeviceContext
*/

//контекст устройства вывода
HDC PrimarySwapChain::GetDC ()
{
  return impl->output_context;
}

//есть ли вертикальная синхронизация
bool PrimarySwapChain::HasVSync ()
{
  return impl->desc.vsync;
}

//получение таблицы WGL-расширений
const WglExtensionEntries& PrimarySwapChain::GetWglExtensionEntries ()
{
  return impl->wgl_extension_entries;
}

/*
    Получение дескриптора
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    Список свойств цепочки обмена
*/

IPropertyList* PrimarySwapChain::GetProperties ()
{
  return &impl->properties;
}

/*
    Получение устройства вывода с максимальным размером области перекрытия
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  return impl->adapter->FindContainingOutput (impl->output_window);
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void PrimarySwapChain::Present ()
{
  try
  {
    impl->adapter->SwapBuffers (impl->output_context);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::PrimarySwapChain::Present");

    throw;
  }
}

/*
    Установка / взятие состояния full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  impl->SetFullscreenState (state, impl->desc.frame_buffer.color_bits);
}

bool PrimarySwapChain::GetFullscreenState ()
{
  return impl->GetFullscreenState ();
}

/*
    Подписка на событие измененения видео-режима
*/

xtl::connection PrimarySwapChain::RegisterDisplayModeChangeHandler (const EventHandler& handler)
{
  try
  {
    return impl->cdm_signal.connect (handler);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::PrimarySwapChain::RegisterDisplayModeChangeHandler");
    throw;
  }
}
