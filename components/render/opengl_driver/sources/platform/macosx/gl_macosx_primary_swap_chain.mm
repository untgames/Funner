#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::macosx;

namespace
{

static void* current_swap_chain = 0;  //current active swap chain

//Interface listening for view frame change notifications
class IViewFrameChangeListener
{
  public:
    virtual void OnViewFrameChanged () = 0;

  protected:
    virtual ~IViewFrameChangeListener () {}
};

}

@interface ViewFrameChangeHandler : NSObject
{
  @private
    IViewFrameChangeListener* listener;
}

@end

@implementation ViewFrameChangeHandler

-(void)dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

-(id)init
{
  self = [super init];
  [self release];

  [NSException raise:@"Invalid operation" format:@"Can't initialize class without view and listener"];

  return nil;
}

-(id)initWithView:(NSView*)view listener:(IViewFrameChangeListener*)inListener
{
  self = [super init];

  if (!self)
    return nil;

  listener = inListener;

  view.postsFrameChangedNotifications = YES;

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector (onFrameChanged) name:NSViewFrameDidChangeNotification object:view];

  return self;
}

-(void)onFrameChanged
{
  if (!listener)
    return;

  listener->OnViewFrameChanged ();
}

@end

/*
    Primary Swap Chain implementation
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;

struct PrimarySwapChain::Impl : public IViewFrameChangeListener
{
  Log                       log;                         //log
  AdapterPtr                adapter;                     //swap chain adapter
  SwapChainDesc             desc;                        //swap chain desc
  NSOpenGLPixelFormat*      pixel_format;                //pixel format
  OutputManager             output_manager;              //output manager
  NSOpenGLContext*          context;                     //opengl context
//TODO check if it's needed  AGLContext                dummy_context;             //dummy context which is needed if we want to use same context for two windows, this allows us to preserve window state while drawing to other window (http://lists.apple.com/archives/Mac-opengl/2004/Mar/msg00210.html)
  Output*                   containing_output;           //display containing output
  size_t                    window_width;                //window width
  size_t                    window_height;               //window height
  xtl::trackable::slot_type on_destroy_context;          //context destroy handler
  ViewFrameChangeHandler*   view_frame_change_handler;   //view frame changed event handler

///Constructor
  Impl (const SwapChainDesc& in_desc, Adapter* in_adapter)
    : adapter (in_adapter),
      pixel_format (0),
      context (0),
      on_destroy_context (xtl::bind (&Impl::OnDestroyContext, this)),
      view_frame_change_handler (0)
  {
      //check window handle

    if (!in_desc.window_handle)
      throw xtl::make_null_argument_exception ("", "in_desc.window_handle");

      //Find containing output

    containing_output = output_manager.FindContainingOutput ((WindowRef)in_desc.window_handle);

      //Choose pixel format

    static const size_t MAX_ATTRIBUTES_COUNT = 64;

    NSOpenGLPixelFormatAttribute attributes [MAX_ATTRIBUTES_COUNT], *attr = attributes;

    *attr++ = NSOpenGLPFAClosestPolicy;
    *attr++ = NSOpenGLPFAOpenGLProfile;
    *attr++ = NSOpenGLProfileVersionLegacy;
    *attr++ = NSOpenGLPFAWindow;

    if (in_desc.buffers_count > 1)
      *attr++ = NSOpenGLPFADoubleBuffer;

    *attr++ = NSOpenGLPFAColorSize;
    *attr++ = in_desc.frame_buffer.color_bits;
    *attr++ = NSOpenGLPFAAlphaSize;
    *attr++ = in_desc.frame_buffer.alpha_bits;
    *attr++ = NSOpenGLPFADepthSize;
    *attr++ = in_desc.frame_buffer.depth_bits;
    *attr++ = NSOpenGLPFAStencilSize;
    *attr++ = in_desc.frame_buffer.stencil_bits;

    if (in_desc.fullscreen && containing_output)
    {
      *attr++ = NSOpenGLPFAScreenMask;
      *attr++ = CGDisplayIDToOpenGLDisplayMask ((CGDirectDisplayID)(size_t)containing_output->Handle ());
    }

    if (in_desc.samples_count)
    {
      *attr++ = NSOpenGLPFAMultisample;
      *attr++ = NSOpenGLPFASampleBuffers;
      *attr++ = 1;
      *attr++ = NSOpenGLPFASamples;
      *attr++ = in_desc.samples_count;
    }

    *attr++ = (NSOpenGLPixelFormatAttribute)0;

    log.Printf ("...call aglChoosePixelFormat");

    pixel_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];

    if (!pixel_format)
      throw xtl::format_operation_exception ("render::low_level::opengl::macosx::PrimarySwapChain::Impl::Impl", "NSOpenGLPixelFormat can't init with requested attributes");

      //check actual pixel format parameters

    try
    {
      memset (&desc, 0, sizeof (desc));

      NSWindow* window = (NSWindow*)in_desc.window_handle;

      NSRect window_rect = window.frame;

      window_width  = window_rect.size.width * window.backingScaleFactor;
      window_height = window_rect.size.height * window.backingScaleFactor;

      window_rect = window.contentView.frame;

      desc.frame_buffer.width        = window_rect.size.width * window.backingScaleFactor;
      desc.frame_buffer.height       = window_rect.size.height * window.backingScaleFactor;
      desc.frame_buffer.color_bits   = static_cast<size_t> (GetPixelFormatValue (NSOpenGLPFAColorSize));
      desc.frame_buffer.alpha_bits   = static_cast<size_t> (GetPixelFormatValue (NSOpenGLPFAAlphaSize));
      desc.frame_buffer.depth_bits   = static_cast<size_t> (GetPixelFormatValue (NSOpenGLPFADepthSize));
      desc.frame_buffer.stencil_bits = static_cast<size_t> (GetPixelFormatValue (NSOpenGLPFAStencilSize));
      desc.samples_count             = static_cast<size_t> (GetPixelFormatValue (NSOpenGLPFASamples));
      desc.buffers_count             = GetPixelFormatValue (NSOpenGLPFADoubleBuffer) ? 2 : 1;
      desc.swap_method               = SwapMethod_Flip;
      desc.vsync                     = in_desc.vsync;
      desc.fullscreen                = false;                 //fullscreen should be used as fullscreen window. Screen mode change is not recommended by Apple (https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/EnablingOpenGLforHighResolution/EnablingOpenGLforHighResolution.html#//apple_ref/doc/uid/TP40001987-CH1001-SW12)
      desc.window_handle             = in_desc.window_handle;

      stl::string flags;

      if (desc.buffers_count > 1)
      {
        flags += ", SwapMethod=";
        flags += get_name (desc.swap_method);
        flags += ", DOUBLE_BUFFER";
      }

      view_frame_change_handler = [[ViewFrameChangeHandler alloc] initWithView:((NSWindow*)in_desc.window_handle).contentView listener:this];

      log.Printf ("...choose %s pixel format (RGB/A: %u/%u, D/S: %u/%u, Samples: %u%s)", GetPixelFormatValue (NSOpenGLPFAAccelerated) ? "HW" : "SW",
        desc.frame_buffer.color_bits, desc.frame_buffer.alpha_bits, desc.frame_buffer.depth_bits,
        desc.frame_buffer.stencil_bits, desc.samples_count, flags.c_str ());

//      dummy_context = aglCreateContext (pixel_format, 0);                           // create a context to hold buffers for the window
//      aglSetDrawable (dummy_context, GetWindowPort ((WindowRef)desc.window_handle)); // force creation of buffers for window
    }
    catch (...)
    {
      [pixel_format release];

      throw;
    }
  }

///Destructor
  ~Impl ()
  {
    log.Printf ("...destroy pixel format");

    [view_frame_change_handler release];
    [pixel_format release];
    [context release];

    log.Printf ("...release resources");
  }

  ///Context destroy handler
  void OnDestroyContext ()
  {
    [context release];
    context = 0;
  }

  ///Swap buffers
  void Present ()
  {
    if (desc.buffers_count  < 2)
      return;

    if (!context)
      return;

    if (current_swap_chain != this)  //TODO test if this is needed
      throw xtl::format_operation_exception("render::low_level::opengl::macosx::PrimarySwapChain::Impl::Present", "Can't present this swap chain now, it is not active");

    [context flushBuffer];
  }

///Get pixel format attribute value
  GLint GetPixelFormatValue (GLint attribute)
  {
    try
    {
      GLint result = 0;

      [pixel_format getValues:&result forAttribute:attribute forVirtualScreen:0];

      return result;
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::Impl::GetPixelFormatValue(%04x)", attribute);
      throw;
    }
  }

  void OnViewFrameChanged ()
  {
    [context update];
  }
};

/*
    Constructor / destructor
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
    //ignore any exceptions
  }
}

/*
    Get adapter
*/

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    Get description
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    Get containing output
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  return impl->output_manager.FindContainingOutput ((NSWindow*)impl->desc.window_handle);
}

/*
    Present
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
    Change fullscreen state
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
   Set new context
*/

void PrimarySwapChain::SetContext (Context* context)
{
  NSOpenGLContext* new_context = context->GetNSOpenGLContext ();

  [impl->context release];
  impl->context = [new_context retain];

  context->RegisterDestroyHandler (impl->on_destroy_context);

  try
  {
      //Set vsync mode

    GLint swap_interval = impl->desc.vsync ? 1 : 0;

    [new_context setValues:&swap_interval forParameter:NSOpenGLCPSwapInterval];

    //setView should be executed only on main thread
    [new_context performSelectorOnMainThread:@selector(setView:) withObject:((NSWindow*)impl->desc.window_handle).contentView waitUntilDone:NO];
    [new_context performSelectorOnMainThread:@selector(update) withObject:nil waitUntilDone:NO];

    current_swap_chain = impl.get ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::PrimarySwapChain::SetContext");
    throw;
  }
}

NSOpenGLPixelFormat* PrimarySwapChain::GetPixelFormat ()
{
  return impl->pixel_format;
}
