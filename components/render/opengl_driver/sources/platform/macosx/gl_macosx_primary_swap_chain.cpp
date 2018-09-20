#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::macosx;

namespace
{

const OSType WINDOW_PROPERTY_CREATOR = 'untg';  //С‚РµРі РїСЂРёР»РѕР¶РµРЅРёСЏ
const OSType FULLSCREEN_PROPERTY_TAG = 'fscr';  //С‚РµРі СЃРІРѕР№СЃС‚РІР° РїРѕР»РЅРѕСЌРєСЂР°РЅРЅРѕСЃС‚Рё

static GLint global_context_buffer_name = 1;  //context buffer name to use for new context
static void* current_swap_chain         = 0;  //current active agl swap chain

}

/*
    РћРїРёСЃР°РЅРёРµ СЂРµР°Р»РёР·Р°С†РёРё PrimarySwapChain
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;

struct PrimarySwapChain::Impl
{
  Log                       log;                       //РїСЂРѕС‚РѕРєРѕР»
  AdapterPtr                adapter;                   //Р°РґР°РїС‚РµСЂ С†РµРїРѕС‡РєРё РѕР±РјРµРЅР°
  SwapChainDesc             desc;                      //РґРµСЃРєСЂРёРїС‚РѕСЂ С†РµРїРѕС‡РєРё РѕР±РјРµРЅР°
  AGLPixelFormat            pixel_format;              //С„РѕСЂРјР°С‚ РїРёРєСЃРµР»РµР№
  OutputManager             output_manager;            //РјРµРЅРµРґР¶РµСЂ СѓСЃС‚СЂРѕР№СЃС‚РІ РІС‹РІРѕРґР°
  AGLContext                context;                   //СѓСЃС‚Р°РЅРѕРІР»РµРЅРЅС‹Р№ РєРѕРЅС‚РµРєСЃС‚
  AGLContext                dummy_context;             //dummy context which is needed if we want to use same context for two windows, this allows us to preserve window state while drawing to other window (http://lists.apple.com/archives/Mac-opengl/2004/Mar/msg00210.html)
  Output*                   containing_output;         //РґРёСЃРїР»РµР№, РЅР° РєРѕС‚РѕСЂРѕРј РїСЂРѕРёР·РІРѕРґРёС‚СЃСЏ СЂРµРЅРґРµСЂРёРЅРі
  EventHandlerRef           window_event_handler;      //РѕР±СЂР°Р±РѕС‚С‡РёРє СЃРѕР±С‹С‚РёР№ РѕРєРЅР°
  EventHandlerUPP           window_event_handler_proc; //РѕР±СЂР°Р±РѕС‚С‡РёРє СЃРѕР±С‹С‚РёР№ РѕРєРЅР°
  size_t                    window_width;              //С€РёСЂРёРЅР° РѕРєРЅР°
  size_t                    window_height;             //РІС‹СЃРѕС‚Р° РѕРєРЅР°
  xtl::trackable::slot_type on_destroy_context;        //РѕР±СЂР°Р±РѕС‚С‡РёРє СѓРґР°Р»РµРЅРёСЏ РєРѕРЅС‚РµРєСЃС‚Р°
  GLint                     context_buffer_name;       //agl buffer name for this swap chain

///Constructor
  Impl (const SwapChainDesc& in_desc, Adapter* in_adapter)
    : adapter (in_adapter), pixel_format (0), context (0), window_event_handler (0), window_event_handler_proc (0),
      on_destroy_context (xtl::bind (&Impl::OnDestroyContext, this)),
      context_buffer_name (global_context_buffer_name++)
  {
      //РїСЂРѕРІРµСЂРєР° РєРѕСЂСЂРµРєС‚РЅРѕСЃС‚Рё Р°СЂРіСѓРјРµРЅС‚РѕРІ

    if (!in_desc.window_handle)
      throw xtl::make_null_argument_exception ("", "in_desc.window_handle");

      //РїРѕРґРїРёСЃРєР° РЅР° СЃРѕР±С‹С‚РёРµ РёР·РјРµРЅРµРЅРёСЏ РѕРєРЅР°

    window_event_handler_proc = NewEventHandlerUPP (&PrimarySwapChain::Impl::window_message_handler);

    EventTypeSpec handled_event_types [] = {
      { kEventClassWindow, kEventWindowBoundsChanged },
    };

    check_event_manager_error (InstallEventHandler (GetWindowEventTarget ((WindowRef)in_desc.window_handle), window_event_handler_proc,
      sizeof (handled_event_types) / sizeof (handled_event_types[0]), handled_event_types,
      this, &window_event_handler), "::InstallEventHandler", "Can't install window event handler");

      //РїРѕРёСЃРє РґРёСЃРїР»РµСЏ, РЅР° РєРѕС‚РѕСЂРѕРј РїСЂРѕРёР·РІРѕРґРёС‚СЃСЏ СЂРµРЅРґРµСЂРёРЅРі

    containing_output = output_manager.FindContainingOutput ((WindowRef)in_desc.window_handle);

      //РІС‹Р±РѕСЂ С„РѕСЂРјР°С‚Р° РїРёРєСЃРµР»РµР№

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
#ifdef AGL_DISPLAY_MASK  //MacOSX 10.5 Рё СЃС‚Р°СЂС€Рµ
      *attr++ = AGL_DISPLAY_MASK;
      *attr++ = CGDisplayIDToOpenGLDisplayMask ((CGDirectDisplayID)containing_output->Handle ());
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

#ifdef AGL_DISPLAY_MASK  //MacOSX 10.5 Рё СЃС‚Р°СЂС€Рµ
    pixel_format = aglChoosePixelFormat (0, 0, attributes);
#else
    if (in_desc.fullscreen && containing_output)
    {
      GDHandle display_device;

      if (noErr != DMGetGDeviceByDisplayID ((DisplayIDType)containing_output->GetDisplayID (), &display_device, false))
        throw xtl::format_operation_exception ("::DMGetGDeviceByDisplayID", "Can't get display device");

      pixel_format = aglChoosePixelFormat (&display_device, 1, attributes);
    }
    else
      pixel_format = aglChoosePixelFormat (0, 0, attributes);
#endif

    if (!pixel_format)
      raise_agl_error ("::aglChoosePixelFormat");

      //РїРѕР»СѓС‡РµРЅРёРµ РїР°СЂР°РјРµС‚СЂРѕРІ РІС‹Р±СЂР°РЅРЅРѕРіРѕ С„РѕСЂРјР°С‚Р° РїРёРєСЃРµР»РµР№

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
          //СѓСЃС‚Р°РЅРѕРІРєР° СЂРµР¶РёРјР° СѓСЃС‚СЂРѕР№СЃС‚РІР° РІС‹РІРѕРґР°

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

      dummy_context = aglCreateContext (pixel_format, 0);                           // create a context to hold buffers for the window
      aglSetInteger (dummy_context, AGL_BUFFER_NAME, &context_buffer_name);         // set buffer name for this window context
      aglSetDrawable(dummy_context, GetWindowPort ((WindowRef)desc.window_handle)); // force creation of buffers for window
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

///Р”РµСЃС‚СЂСѓРєС‚РѕСЂ
  ~Impl ()
  {
    log.Printf ("...destroy pixel format");

/*    if (desc.fullscreen)
    {
        //РІРѕСЃСЃС‚Р°РЅРѕРІР»РµРЅРёРµ РЅР°С‡Р°Р»СЊРЅРѕРіРѕ СЂРµР¶РёРјР° СѓСЃС‚СЂРѕР№СЃС‚РІР° РІС‹РІРѕРґР°

      containing_output->SetCurrentMode (default_output_mode);
    }*/

    RemoveEventHandler     (window_event_handler);
    DisposeEventHandlerUPP (window_event_handler_proc);

    aglDestroyPixelFormat (pixel_format);

    log.Printf ("...release resources");
  }

  ///РћР±СЂР°Р±РѕС‚С‡РёРє СѓРґР°Р»РµРЅРёСЏ РєРѕРЅС‚РµРєСЃС‚Р°
  void OnDestroyContext ()
  {
    context = 0;
  }

  ///РћР±РјРµРЅ С‚РµРєСѓС‰РµРіРѕ Р·Р°РґРЅРµРіРѕ Р±СѓС„РµСЂР° Рё РїРµСЂРµРґРЅРµРіРѕ Р±СѓС„РµСЂР°
  void Present ()
  {
    if (desc.buffers_count  < 2)
      return;

    if (!context)
      return;

    if (current_swap_chain != this)
      throw xtl::format_operation_exception("render::low_level::opengl::macosx::PrimarySwapChain::Impl::Present", "Can't present this swap chain now, it is not active");

    aglSwapBuffers (context);

    check_agl_error ("::aglSwapBuffers");
  }

///РџРѕР»СѓС‡РµРЅРёРµ Р·РЅР°С‡РµРЅРёР№ Р°С‚СЂРёР±СѓС‚РѕРІ
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
    РљРѕРЅСЃС‚СЂСѓРєС‚РѕСЂ / РґРµСЃС‚СЂСѓРєС‚РѕСЂ
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
    //РїРѕРґР°РІР»СЏРµРј РІСЃРµ РёСЃРєР»СЋС‡РµРЅРёСЏ
  }
}

/*
    РџРѕР»СѓС‡РµРЅРёРµ Р°РґР°РїС‚РµСЂР°
*/

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    РџРѕР»СѓС‡РµРЅРёРµ РґРµСЃРєСЂРёРїС‚РѕСЂР°
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    РџРѕР»СѓС‡РµРЅРёРµ СѓСЃС‚СЂРѕР№СЃС‚РІР° РІС‹РІРѕРґР° СЃ РјР°РєСЃРёРјР°Р»СЊРЅС‹Рј СЂР°Р·РјРµСЂРѕРј РѕР±Р»Р°СЃС‚Рё РїРµСЂРµРєСЂС‹С‚РёСЏ
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  return impl->output_manager.FindContainingOutput ((WindowRef)impl->desc.window_handle);
}

/*
    РћР±РјРµРЅ С‚РµРєСѓС‰РµРіРѕ Р·Р°РґРЅРµРіРѕ Р±СѓС„РµСЂР° Рё РїРµСЂРµРґРЅРµРіРѕ Р±СѓС„РµСЂР°
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
    РЈСЃС‚Р°РЅРѕРІРєР° / РІР·СЏС‚РёРµ СЃРѕСЃС‚РѕСЏРЅРёСЏ full-screen mode
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
   РџРѕР»СѓС‡РµРЅРёРµ/СѓСЃС‚Р°РЅРѕРІРєР° РєРѕРЅС‚РµРєСЃС‚Р°
*/

void PrimarySwapChain::SetContext (Context* context)
{
  AGLContext new_context = context->GetAGLContext ();

  impl->context = new_context;

  aglSetInteger (impl->context, AGL_BUFFER_NAME, &impl->context_buffer_name);          // set buffer name for this window context

  context->RegisterDestroyHandler (impl->on_destroy_context);

  try
  {
      //СѓСЃС‚Р°РЅРѕРІРєР° vsync

    GLint swap_interval = impl->desc.vsync ? 1 : 0;

    if (!aglSetInteger (new_context, AGL_SWAP_INTERVAL, &swap_interval))
      raise_agl_error ("::aglSetInteger");

      //СѓСЃС‚Р°РЅРѕРІРєР° fullscreen

    check_window_manager_error (SetWindowProperty ((WindowRef)impl->desc.window_handle, WINDOW_PROPERTY_CREATOR, FULLSCREEN_PROPERTY_TAG,
                                sizeof (impl->desc.fullscreen), &impl->desc.fullscreen), "::SetWindowProperty");

    if (impl->desc.fullscreen)
    {
      if (!aglSetFullScreen (new_context, impl->window_width, impl->window_height, 0, 0))
        raise_agl_error ("::aglSetFullScreen");
    }
    else
    {
      if (!aglSetWindowRef (new_context, (WindowRef)impl->desc.window_handle))
        raise_agl_error ("::aglSetWindowRef");

      aglUpdateContext (new_context);
    }

    current_swap_chain = impl.get ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::SetContext");
    throw;
  }
}

AGLPixelFormat PrimarySwapChain::GetPixelFormat ()
{
  return impl->pixel_format;
}
