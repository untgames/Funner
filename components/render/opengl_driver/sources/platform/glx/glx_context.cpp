#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    Context implementation
*/

typedef stl::vector<IContextListener*> ListenerArray;
typedef xtl::com_ptr<Adapter>          AdapterPtr;

struct Context::Impl
{  
  Log                         log;                   //render context log
  AdapterPtr                  adapter;               //target rendering adapter
  Display*                    display;               //display device for current context
  Window                      window;                //render window
  GLXContext                  glx_context;           //GLX-rendering context
  ListenerArray               listeners;             //context events listeners
  xtl::trackable::slot_type   on_destroy_swap_chain; //swap chain destroy handler
  ISwapChainImpl*             swap_chain;            //current swap chain
  const GlxExtensionsEntries* glx_extensions;        //GLX extensions
  bool                        vsync;                 //vsync
  static Impl*                current_context;       //current context
  
///Constructor
  Impl ()
    : on_destroy_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this))
    , swap_chain (0)
    , glx_extensions ()
    , vsync (false)
  {    
  }
  
///Set vsync mode
  void SetVSync ()
  {
    if (glx_extensions->SwapIntervalSGI)
      glx_extensions->SwapIntervalSGI (vsync);
  }  
  
///Reset current context
  void ResetContext ()
  {
    if (current_context != this)
      return;

    LostCurrentNotify ();
    
    DisplayLock lock (display);
      
    adapter->GetLibrary ().MakeCurrent (display, None, NULL);

    current_context = 0;
  }

///Swap chain destroy handler
  void OnDestroySwapChain ()
  {    
    if (current_context == this)
      ResetContext ();
    
    swap_chain     = 0;
    display        = 0;
    vsync          = false;
    glx_extensions = 0;
  }

///Context lost notification
  void LostCurrentNotify ()
  {
    try
    {
      for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
        (*iter)->OnLostCurrent ();
    }
    catch (...)
    {
      //ignore all exceptions
    }
  }

///Current context setted notification
  void SetCurrentNotify ()
  {
    try
    {
      for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
        (*iter)->OnSetCurrent ();
    }
    catch (...)
    {
      //ignore all exceptions
    }
  }
};

Context::Impl* Context::Impl::current_context = 0;

/*
    Constructor / destructor
*/

Context::Context (ISwapChain* in_swap_chain)
{
  try
  {
      //check arguments correctness
      
    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    ISwapChainImpl* swap_chain = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");    

      //create implementation

    impl = new Impl;    

    impl->adapter  = cast_object<Adapter> (swap_chain->GetAdapter (), "", "adapter");
    impl->display  = swap_chain->GetDisplay ();
    impl->window   = swap_chain->GetWindow ();
    
    DisplayLock lock (impl->display);

      //create context

    impl->log.Printf ("Create context (id=%d)...", GetId ());
    
    impl->glx_context = impl->adapter->GetLibrary ().CreateContext (impl->display,
                                                                    swap_chain->GetFBConfig (),
                                                                    GLX_RGBA_TYPE, 0, True);
                                                                    
    if (!impl->glx_context)
      raise_error ("::glxCreateContext");
      
      //log context created
    
    impl->log.Printf ("...context successfully created (handle=%08x)", impl->glx_context);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Context::Context");
    throw;
  }
}

Context::~Context ()
{
  try
  {
    impl->log.Printf ("Delete context (id=%d)...", GetId ());
    
      //reset current context

    if (Impl::current_context == impl.get ())
      impl->ResetContext ();

    Display* display = impl->display ? impl->display : (Display*)DisplayManager::DisplayHandle ();  //impl->display may be null at this moment if swap chain was already destroyed

    DisplayLock lock (display);

    impl->log.Printf ("...delete context (handle=%08X)", impl->glx_context);

    impl->adapter->GetLibrary ().DestroyContext (display, impl->glx_context);

    impl->log.Printf ("...context successfully destroyed");
  }
  catch (...)
  {
    //ignore all exceptions
  }
}

/*
    Set current context
*/

void Context::MakeCurrent (ISwapChain* swap_chain)
{
  try
  {
    if (!swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    if (swap_chain != impl->swap_chain)
    {
        //Change current swap chain

      ISwapChainImpl* casted_swap_chain = cast_object<ISwapChainImpl> (swap_chain, "", "swap_chain");

        //Subscribe for swap chain destroy event

      casted_swap_chain->RegisterDestroyHandler (impl->on_destroy_swap_chain);

      impl->swap_chain     = casted_swap_chain;
      impl->display        = impl->swap_chain->GetDisplay ();
      impl->window         = impl->swap_chain->GetWindow ();
      impl->vsync          = casted_swap_chain->HasVSync ();
      impl->glx_extensions = &casted_swap_chain->GetGlxExtensionsEntries ();
    }
    
      //notify current context lost
    
    if (Impl::current_context)
    {
      Impl::current_context->LostCurrentNotify ();    
      
      impl->current_context = 0;
    }
    
    DisplayLock lock (impl->display);
    
      //set current context
      
    if (!impl->adapter->GetLibrary ().MakeCurrent (impl->display, impl->window, impl->glx_context))
      raise_error ("::glxMakeContextCurrent");
      
    Impl::current_context = impl.get ();        
    
      //set vsync

    impl->SetVSync ();    

      //notify current context set

    impl->SetCurrentNotify ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Context::MakeCurrent");
    throw;
  }

}

/*
    Check swap chain compatibility
*/

bool Context::IsCompatible (ISwapChain* in_swap_chain)
{
  try
  {
    ISwapChainImpl* swap_chain = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");

    return swap_chain->GetDisplay () == impl->display; //TODO most probably this is wrong, should be checked!!!
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Context::IsCompatible");
    throw;
  }
}

/*
    Get OpenGL library interface
*/

AdapterLibrary& Context::GetLibrary ()
{
  return impl->adapter->GetLibrary ();
}

/*
    Subscribe for context events
*/

void Context::AttachListener (IContextListener* listener)
{
  if (!listener)
    return;

  impl->listeners.push_back (listener);
}

void Context::DetachListener (IContextListener* listener)
{
  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}
