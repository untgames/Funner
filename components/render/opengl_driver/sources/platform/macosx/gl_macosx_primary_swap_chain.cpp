#if ! defined (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) || (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1040)
  #error "__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ not defined or less then 1040"
#endif

#if (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050)
  #define MACOSX_10_5_SUPPORTED
#endif

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::macosx;

namespace
{

const OSType WINDOW_PROPERTY_CREATOR = 'untg';  //тег приложения
const OSType FULLSCREEN_PROPERTY_TAG = 'fscr';  //тег свойства полноэкранности

}

/*
    Описание реализации PrimarySwapChain
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;

struct PrimarySwapChain::Impl
{
  Log             log;                       //протокол
  AdapterPtr      adapter;                   //адаптер цепочки обмена
  SwapChainDesc   desc;                      //дескриптор цепочки обмена
  AGLPixelFormat  pixel_format;              //формат пикселей
  OutputManager   output_manager;            //менеджер устройств вывода
  AGLContext      context;                   //установленный контекст
  Output*         containing_output;         //дисплей, на котором производится рендеринг
  EventHandlerRef window_event_handler;      //обработчик событий окна
  EventHandlerUPP window_event_handler_proc; //обработчик событий окна
  size_t          window_width;              //ширина окна
  size_t          window_height;             //высота окна

///Конструктор
  Impl (const SwapChainDesc& in_desc, Adapter* in_adapter)
    : adapter (in_adapter), pixel_format (0), context (0), window_event_handler (0), window_event_handler_proc (0)
  {
      //проверка корректности аргументов

    if (!in_desc.window_handle)
      throw xtl::make_null_argument_exception ("", "in_desc.window_handle");

      //подписка на событие изменения окна

    window_event_handler_proc = NewEventHandlerUPP (&PrimarySwapChain::Impl::window_message_handler);

    EventTypeSpec handled_event_types [] = {
      { kEventClassWindow, kEventWindowBoundsChanged },
    };

    check_event_manager_error (InstallEventHandler (GetWindowEventTarget ((WindowRef)in_desc.window_handle), window_event_handler_proc,
      sizeof (handled_event_types) / sizeof (handled_event_types[0]), handled_event_types,
      this, &window_event_handler), "::InstallEventHandler", "Can't install window event handler");

      //поиск дисплея, на котором производится рендеринг

    containing_output = output_manager.FindContainingOutput ((WindowRef)in_desc.window_handle);

      //выбор формата пикселей

    static const size_t MAX_ATTRIBUTES_COUNT = 64;

    GLint attributes [MAX_ATTRIBUTES_COUNT], *attr = attributes;

    *attr++ = AGL_COMPLIANT;
    *attr++ = AGL_WINDOW;
    *attr++ = AGL_RGBA;

    if (in_desc.buffers_count > 1)
      *attr++ = AGL_DOUBLEBUFFER;

    *attr++ = AGL_PIXEL_SIZE;
    *attr++ = in_desc.frame_buffer.color_bits;
    *attr++ = AGL_ALPHA_SIZE;
    *attr++ = in_desc.frame_buffer.alpha_bits;
    *attr++ = AGL_DEPTH_SIZE;
    *attr++ = in_desc.frame_buffer.depth_bits;
    *attr++ = AGL_STENCIL_SIZE;
    *attr++ = in_desc.frame_buffer.stencil_bits;

    if (in_desc.fullscreen && containing_output)
    {
      *attr++ = AGL_FULLSCREEN;
#ifdef AGL_DISPLAY_MASK  //MacOSX 10.5 и старше
      *attr++ = AGL_DISPLAY_MASK;
      *attr++ = CGDisplayIDToOpenGLDisplayMask (containing_output->GetDisplayID ());
#endif
    }

    if (in_desc.samples_count)
    {
      *attr++ = AGL_SAMPLE_BUFFERS_ARB;
      *attr++ = 1;
      *attr++ = AGL_SAMPLES_ARB;
      *attr++ = in_desc.samples_count;
      *attr++ = AGL_SUPERSAMPLE;
    }

    *attr++ = AGL_NONE;

    log.Printf ("...call aglChoosePixelFormat");

    pixel_format = aglChoosePixelFormat (0, 0, attributes);

    if (!pixel_format)
      raise_agl_error ("::aglChoosePixelFormat");

      //получение параметров выбранного формата пикселей

    try
    {
      memset (&desc, 0, sizeof (desc));

      ::Rect window_rect;

      check_window_manager_error (GetWindowBounds ((WindowRef)in_desc.window_handle, kWindowStructureRgn, &window_rect), "::GetWindowBounds");

      window_width  = window_rect.right - window_rect.left;
      window_height = window_rect.bottom - window_rect.top;

      check_window_manager_error (GetWindowBounds ((WindowRef)in_desc.window_handle, kWindowContentRgn, &window_rect), "::GetWindowBounds");

      desc.frame_buffer.width        = window_rect.right - window_rect.left;
      desc.frame_buffer.height       = window_rect.bottom - window_rect.top;
      desc.frame_buffer.color_bits   = static_cast<size_t> (GetPixelFormatValue (AGL_PIXEL_SIZE));
      desc.frame_buffer.alpha_bits   = static_cast<size_t> (GetPixelFormatValue (AGL_ALPHA_SIZE));
      desc.frame_buffer.depth_bits   = static_cast<size_t> (GetPixelFormatValue (AGL_DEPTH_SIZE));
      desc.frame_buffer.stencil_bits = static_cast<size_t> (GetPixelFormatValue (AGL_STENCIL_SIZE));
      desc.samples_count             = static_cast<size_t> (GetPixelFormatValue (AGL_SAMPLES_ARB));
      desc.buffers_count             = GetPixelFormatValue (AGL_DOUBLEBUFFER) ? 2 : 1;
      desc.swap_method               = SwapMethod_Flip;
      desc.vsync                     = in_desc.vsync;
      desc.fullscreen                = GetPixelFormatValue (AGL_FULLSCREEN) != 0;
      desc.window_handle             = in_desc.window_handle;

      stl::string flags;

      if (desc.buffers_count > 1)
      {
        flags += ", SwapMethod=";
        flags += get_name (desc.swap_method);
        flags += ", DOUBLE_BUFFER";
      }

      if (desc.fullscreen)
      {
        if (flags.empty ()) flags += ", ";
        else                flags += " | ";

        flags += "FULLSCREEN";
      }

/*      if (desc.fullscreen)
      {
          //установка режима устройства вывода

        containing_output->GetCurrentMode (default_output_mode);

        OutputModeDesc fullscreen_mode;

        memset (&fullscreen_mode, 0, sizeof (fullscreen_mode));

        printf ("Width = %u, height = %u, color_bits = %u\n", desc.frame_buffer.width, desc.frame_buffer.height, desc.frame_buffer.color_bits);

        fullscreen_mode.width      = desc.frame_buffer.width;
        fullscreen_mode.height     = desc.frame_buffer.height;
        fullscreen_mode.color_bits = desc.frame_buffer.color_bits;

        containing_output->SetCurrentMode (fullscreen_mode);
      }*/

      log.Printf ("...choose %s pixel format (RGB/A: %u/%u, D/S: %u/%u, Samples: %u%s)", GetPixelFormatValue (AGL_ACCELERATED) ? "HW" : "SW",
        desc.frame_buffer.color_bits, desc.frame_buffer.alpha_bits, desc.frame_buffer.depth_bits,
        desc.frame_buffer.stencil_bits, desc.samples_count, flags.c_str ());

    }
    catch (...)
    {
      if (window_event_handler)
        RemoveEventHandler (window_event_handler);

      if (window_event_handler_proc)
        DisposeEventHandlerUPP (window_event_handler_proc);

      if (pixel_format)
        aglDestroyPixelFormat (pixel_format);

      throw;
    }
  }

///Деструктор
  ~Impl ()
  {
    log.Printf ("...destroy pixel format");

/*    if (desc.fullscreen)
    {
        //восстановление начального режима устройства вывода

      containing_output->SetCurrentMode (default_output_mode);
    }*/

    RemoveEventHandler     (window_event_handler);
    DisposeEventHandlerUPP (window_event_handler_proc);

    aglDestroyPixelFormat (pixel_format);

    log.Printf ("...release resources");
  }

  ///Обмен текущего заднего буфера и переднего буфера
  void Present ()
  {
    if (desc.buffers_count  < 2)
      return;

    if (!context)
      return;

    aglSwapBuffers (context);

    check_agl_error ("::aglSwapBuffers");
  }

///Получение значений атрибутов
  GLint GetPixelFormatValue (GLint attribute)
  {
    try
    {
      GLint result = 0;

      if (!aglDescribePixelFormat (pixel_format, attribute, &result))
        raise_agl_error ("::aglDescribePixelFormat");

      return result;
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::Impl::GetPixelFormatValue(%04x)", attribute);
      throw;
    }
  }

  static OSStatus window_message_handler (EventHandlerCallRef event_handler_call_ref, EventRef event, void* swap_chain_impl)
  {
    if ((GetEventClass (event) != kEventClassWindow) || (GetEventKind (event) != kEventWindowBoundsChanged))
      return eventNotHandledErr;

    Impl* this_impl = (Impl*)swap_chain_impl;

    if (this_impl->context)
      aglUpdateContext (this_impl->context);

    try
    {
      OSStatus operation_result = CallNextEventHandler (event_handler_call_ref, event);

      if (operation_result != eventNotHandledErr)
        check_event_manager_error (operation_result, "::CallNextEventHandler", "Can't call next event handler");
    }
    catch (std::exception& exception)
    {
      printf ("Exception at processing event in ::window_message_handler : '%s'\n", exception.what ());
    }
    catch (...)
    {
      printf ("Exception at processing event in ::window_message_handler : unknown exception\n");
    }

    return noErr;
  }
};

/*
    Конструктор / деструктор
*/

PrimarySwapChain::PrimarySwapChain (const SwapChainDesc& desc, Adapter* adapter)
{
  Log log;

  log.Printf ("...create primary swap chain (id=%u)", GetId ());

  try
  {
    impl = new Impl (desc, adapter);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::PrimarySwapChain");
    throw;
  }

  log.Printf ("...primary swap chain successfully created");
}

PrimarySwapChain::~PrimarySwapChain ()
{
  try
  {
    Log log;

    log.Printf ("Destroy primary swap chain (id=%u)...", GetId ());

    impl = 0;

    log.Printf ("...primary swap chain successfully destroyed");
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Получение адаптера
*/

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    Получение дескриптора
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    Получение устройства вывода с максимальным размером области перекрытия
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  return impl->output_manager.FindContainingOutput ((WindowRef)impl->desc.window_handle);
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void PrimarySwapChain::Present ()
{
  try
  {
    impl->Present ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::Present");

    throw;
  }
}

/*
    Установка / взятие состояния full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::macosx::PrimarySwapChain::SetFullscreenState",
                                             "Changing fullscreen state not supported");
}

bool PrimarySwapChain::GetFullscreenState ()
{
  return impl->desc.fullscreen;
}

/*
   Получение/установка контекста
*/

void PrimarySwapChain::SetContext (AGLContext context)
{
  if (impl->context == context)
    return;

  impl->context = context;

  if (!context)
    return;

  try
  {
      //установка vsync

    GLint swap_interval = impl->desc.vsync ? 1 : 0;

    if (!aglSetInteger (context, AGL_SWAP_INTERVAL, &swap_interval))
      raise_agl_error ("::aglSetInteger");

      //установка fullscreen

    check_window_manager_error (SetWindowProperty ((WindowRef)impl->desc.window_handle, WINDOW_PROPERTY_CREATOR, FULLSCREEN_PROPERTY_TAG,
                                sizeof (impl->desc.fullscreen), &impl->desc.fullscreen), "::SetWindowProperty");

    if (impl->desc.fullscreen)
    {
      if (!aglSetFullScreen (context, impl->window_width, impl->window_height, 0, 0))
        raise_agl_error ("::aglSetFullScreen");
    }
    else
    {
#ifdef MACOSX_10_5_SUPPORTED
      if (!aglSetWindowRef (context, (WindowRef)impl->desc.window_handle))
        raise_agl_error ("::aglSetWindowRef");
#else
      if (!aglSetDrawable (context, GetWindowPort ((WindowRef)impl->desc.window_handle)))
        raise_agl_error ("::aglSetDrawable");
#endif

      aglUpdateContext (context);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::SetContext");
    throw;
  }
}

AGLContext PrimarySwapChain::GetContext ()
{
  return impl->context;
}

AGLPixelFormat PrimarySwapChain::GetPixelFormat ()
{
  return impl->pixel_format;
}
