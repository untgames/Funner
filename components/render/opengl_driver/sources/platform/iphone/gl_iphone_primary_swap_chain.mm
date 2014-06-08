#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::iphone;

namespace
{

//Интерфейс реакции на события изменения размеров View
class IViewSizeChangeListener
{
  public:
    virtual void OnViewSizeChanged () = 0;

  protected:
    virtual ~IViewSizeChangeListener () {}
};

}

@interface ViewSizeChangeHandler : UIView
{
  @private
    IViewSizeChangeListener* listener;
}

@end

@implementation ViewSizeChangeHandler

-(id)init
{
  self = [super init];
  [self release];

  [NSException raise:@"Invalid operation" format:@"Can't initialize class without superview and listener"];

  return nil;
}

-(id)initWithFrame:(CGRect)frame
{
  self = [super init];
  [self release];

  [NSException raise:@"Invalid operation" format:@"Can't initialize class without superview and listener"];

  return nil;
}

-(id)initWithSuperview:(UIView*)superview listener:(IViewSizeChangeListener*)inListener
{
  self = [super initWithFrame:superview.bounds];

  if (!self)
    return nil;

  listener = inListener;

  [superview addSubview:self];

  self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
  self.hidden           = true;

  return self;
}

-(void)setFrame:(CGRect)newFrame
{
  if (!listener)
    return;

  [super setFrame:newFrame];

  listener->OnViewSizeChanged ();
}

@end

/*
    Описание реализации PrimarySwapChain
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;

struct PrimarySwapChain::Impl : public IViewSizeChangeListener
{
  Log                    log;                       //протокол
  AdapterPtr             adapter;                   //адаптер цепочки обмена
  SwapChainDesc          desc;                      //дескриптор цепочки обмена
  Context*               context;                   //установленный контекст
  EAGLContext*           eagl_context;              //установленный контекст
  ISwapChain*            swap_chain;                //цепочка обмена
  GLuint                 frame_buffer;              //буфер кадра
  GLuint                 render_buffer;             //буфер рендеринга
  GLuint                 depth_buffer;              //буфер глубины
  GLuint                 stencil_buffer;            //буфер шаблона
  GLuint                 sample_frame_buffer;       //буфер кадра для мультисэмплинга
  GLuint                 sample_render_buffer;      //буфер рендеринга для мультисэмплинга
  GLuint                 sample_depth_buffer;       //буфер глубины для мультисэмплинга
  ViewSizeChangeHandler* view_size_change_handler;  //обработчик изменения размеров view

///Конструктор
  Impl (const SwapChainDesc& in_desc, Adapter* in_adapter, ISwapChain *in_swap_chain)
    : adapter (in_adapter)
    , context (0)
    , eagl_context (0)
    , swap_chain (in_swap_chain)
    , frame_buffer (0)
    , render_buffer (0)
    , depth_buffer (0)
    , stencil_buffer (0)
    , sample_frame_buffer (0)
    , sample_render_buffer (0)
    , sample_depth_buffer (0)
  {
      //проверка корректности аргументов

    if (!in_desc.window_handle)
      throw xtl::make_null_argument_exception ("", "in_desc.window_handle");

      //заполнение дескриптора цепочки обмена

    memset (&desc, 0, sizeof (desc));

    UIView* draw_view = [((UIWindow*)in_desc.window_handle).rootViewController.view retain];

    CGRect window_frame = draw_view.frame;

    desc.frame_buffer.width        = window_frame.size.width;
    desc.frame_buffer.height       = window_frame.size.height;
    desc.frame_buffer.color_bits   = 24;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = in_desc.frame_buffer.depth_bits ? 16 : 0;
    desc.frame_buffer.stencil_bits = in_desc.frame_buffer.stencil_bits ? 8 : 0;
    desc.samples_count             = in_desc.samples_count;
    desc.buffers_count             = 2;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = true;
    desc.fullscreen                = false;
    desc.window_handle             = in_desc.window_handle;

    log.Printf ("...choose pixel format (RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
      desc.frame_buffer.color_bits, desc.frame_buffer.alpha_bits, desc.frame_buffer.depth_bits,
      desc.frame_buffer.stencil_bits, desc.samples_count);

    view_size_change_handler = [[ViewSizeChangeHandler alloc] initWithSuperview:draw_view listener:this];
  }

///Деструктор
  ~Impl ()
  {
    log.Printf ("...release resources");

    [view_size_change_handler release];
  }

  ///Установка нового контекста
  void InitializeForContext (EAGLContext* new_eagl_context, Context* new_context)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::iphone::PrimarySwapChain::Impl::SetContext";

    if (context)
      throw xtl::format_operation_exception (METHOD_NAME, "Can't set two contexts for one swap chain");

    //??????Необходимо учитывать разные имена функций при наличии/отсутсвии расширения

    context      = new_context;
    eagl_context = new_eagl_context;

    log.Printf ("...making context current");

    context->MakeCurrent (swap_chain);

    try
    {
      log.Printf ("...creating framebuffer");

      glGenFramebuffersOES (1, &frame_buffer);
      glBindFramebufferOES (GL_FRAMEBUFFER_OES, frame_buffer);

      log.Printf ("...creating renderbuffer");

      glGenRenderbuffersOES (1, &render_buffer);
      glBindRenderbufferOES (GL_RENDERBUFFER_OES, render_buffer);

      CheckErrors (METHOD_NAME);

      log.Printf ("...binding to drawable");

      CAEAGLLayer* eagl_layer = (CAEAGLLayer*)[((UIWindow*)desc.window_handle).rootViewController.view layer];

      eagl_layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO],
                                       kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

      if (![eagl_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:eagl_layer])
        throw xtl::format_operation_exception ("render::low_level::opengl::iphone::PrimarySwapChain::Impl::SetContext", "Can't set context");

      log.Printf ("...attaching renderbuffer");

      glFramebufferRenderbufferOES (GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, render_buffer);

      GLint renderbuffer_width, renderbuffer_height;

      glGetRenderbufferParameterivOES (GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &renderbuffer_width);
      glGetRenderbufferParameterivOES (GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &renderbuffer_height);

      desc.frame_buffer.width  = renderbuffer_width;
      desc.frame_buffer.height = renderbuffer_height;

      log.Printf ("...creating additional renderbuffers");

      if (desc.frame_buffer.depth_bits)
      {
        glGenRenderbuffersOES (1, &depth_buffer);
        glBindRenderbufferOES (GL_RENDERBUFFER_OES, depth_buffer);
        glRenderbufferStorageOES (GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, desc.frame_buffer.width, desc.frame_buffer.height);
        glFramebufferRenderbufferOES (GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depth_buffer);
      }

      if (desc.frame_buffer.stencil_bits)
      {
        glGenRenderbuffersOES (1, &stencil_buffer);
        glBindRenderbufferOES (GL_RENDERBUFFER_OES, stencil_buffer);
        glRenderbufferStorageOES (GL_RENDERBUFFER_OES, GL_STENCIL_INDEX8_OES, desc.frame_buffer.width, desc.frame_buffer.height);
        glFramebufferRenderbufferOES (GL_FRAMEBUFFER_OES, GL_STENCIL_ATTACHMENT_OES, GL_RENDERBUFFER_OES, stencil_buffer);
      }

      size_t samples_count = desc.samples_count;

      if (samples_count > 1)
      {
        GLuint max_samples_count = 0;

        glGetIntegerv (GL_MAX_SAMPLES_APPLE, (GLint*)&max_samples_count);

        samples_count = stl::min ((GLuint)samples_count, max_samples_count);
      }

      if (samples_count > 1)
      {
        glGenFramebuffers (1, &sample_frame_buffer);
        glBindFramebuffer (GL_FRAMEBUFFER, sample_frame_buffer);

        glGenRenderbuffers (1, &sample_render_buffer);
        glBindRenderbuffer (GL_RENDERBUFFER, sample_render_buffer);
        glRenderbufferStorageMultisampleAPPLE (GL_RENDERBUFFER, samples_count, GL_RGBA8_OES, desc.frame_buffer.width, desc.frame_buffer.height);
        glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, sample_render_buffer);

        if (desc.frame_buffer.depth_bits)
        {
          glGenRenderbuffers (1, &sample_depth_buffer);
          glBindRenderbuffer (GL_RENDERBUFFER, sample_depth_buffer);
          glRenderbufferStorageMultisampleAPPLE (GL_RENDERBUFFER, samples_count, GL_DEPTH_COMPONENT16, desc.frame_buffer.width, desc.frame_buffer.height);
          glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sample_depth_buffer);
        }
      }

      GLenum status = glCheckFramebufferStatusOES (GL_FRAMEBUFFER_OES);

      CheckErrors (METHOD_NAME);

      if (status != GL_FRAMEBUFFER_COMPLETE_OES)
        throw xtl::format_operation_exception (METHOD_NAME, "Failed to make complete framebuffer object");

      log.Printf ("...swap chain initialized");
    }
    catch (...)
    {
      DoneForContext ();
      throw;
    }
  }

  ///Освобождение ресурсов старого контекста
  void DoneForContext ()
  {
    if (!context)
      return;

    context->MakeCurrent (swap_chain);

    context      = 0;
    eagl_context = 0;

    glDeleteFramebuffersOES  (1, &frame_buffer);
    glDeleteRenderbuffersOES (1, &render_buffer);

    if (sample_frame_buffer)
      glDeleteFramebuffersOES  (1, &sample_frame_buffer);

    if (sample_render_buffer)
      glDeleteRenderbuffersOES  (1, &sample_render_buffer);

    if (desc.frame_buffer.depth_bits)
      glDeleteRenderbuffersOES (1, &depth_buffer);

    if (sample_depth_buffer)
      glDeleteRenderbuffersOES  (1, &sample_depth_buffer);

    if (desc.frame_buffer.stencil_bits)
      glDeleteRenderbuffersOES (1, &stencil_buffer);

    frame_buffer         = 0;
    render_buffer        = 0;
    depth_buffer         = 0;
    stencil_buffer       = 0;
    sample_frame_buffer  = 0;
    sample_render_buffer = 0;
    sample_depth_buffer  = 0;

    CheckErrors ("render::low_level::opengl::iphone::PrimarySwapChain::Impl::DoneForContext");
  }

  ///Обмен текущего заднего буфера и переднего буфера
  void Present ()
  {
    static const char* METHOD_NAME = "render::low_level::opengl::iphone::PrimarySwapChain::Impl::Present";

    if (!context)
      return;

    context->MakeCurrent (swap_chain);

    GLuint current_render_buffer;

    glGetIntegerv (GL_RENDERBUFFER_BINDING_OES, (GLint*)&current_render_buffer);

    CheckErrors (METHOD_NAME);

    if (current_render_buffer != render_buffer)
    {
      glBindRenderbufferOES (GL_RENDERBUFFER_OES, render_buffer);

      CheckErrors (METHOD_NAME);
    }

    try
    {
      if (sample_frame_buffer)
      {
        glBindFramebuffer (GL_DRAW_FRAMEBUFFER_APPLE, frame_buffer);
        glBindFramebuffer (GL_READ_FRAMEBUFFER_APPLE, sample_frame_buffer);
        glResolveMultisampleFramebufferAPPLE ();

        CheckErrors (METHOD_NAME);
      }

      if (![eagl_context presentRenderbuffer:GL_RENDERBUFFER_OES])
        throw xtl::format_operation_exception (METHOD_NAME, "Failed to swap renderbuffer");

      if (sample_frame_buffer)
      {
        glBindFramebuffer (GL_FRAMEBUFFER, sample_frame_buffer);

        CheckErrors (METHOD_NAME);
      }
    }
    catch (...)
    {
      if (current_render_buffer != render_buffer)
      {
        glBindRenderbufferOES (GL_RENDERBUFFER_OES, current_render_buffer);
        CheckErrors (METHOD_NAME);
      }

      throw;
    }
  }

  void OnViewSizeChanged ()
  {
    if (!context)
      return;

    CGSize draw_view_size = ((UIWindow*)desc.window_handle).rootViewController.view.bounds.size;

    if (draw_view_size.width == desc.frame_buffer.width && draw_view_size.height == desc.frame_buffer.height)
      return;

    Context*     stored_context      = context;
    EAGLContext* stored_eagl_context = eagl_context;

    DoneForContext ();
    InitializeForContext (stored_eagl_context, stored_context);
  }

  private:
     ///Проверка ошибок OpenGL
    void CheckErrors (const char* source)
    {
      GLenum error = glGetError ();

      switch (error)
      {
        case GL_NO_ERROR:
          break;
        case GL_INVALID_ENUM:
          throw xtl::format_operation_exception (source, "OpenGL error: invalid enum");
        case GL_INVALID_VALUE:
          throw xtl::format_operation_exception (source, "OpenGL error: invalid value");
        case GL_INVALID_OPERATION:
          throw xtl::format_operation_exception (source, "OpenGL error: invalid operation");
        case GL_STACK_OVERFLOW:
          throw xtl::format_operation_exception (source, "OpenGL error: stack overflow");
        case GL_STACK_UNDERFLOW:
          throw xtl::format_operation_exception (source, "OpenGL error: stack underflow");
        case GL_OUT_OF_MEMORY:
          throw xtl::format_operation_exception (source, "OpenGL error: out of memory");
        default:
          throw xtl::format_operation_exception (source, "OpenGL error: code=0x%04x", error);
      }
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
    impl = new Impl (desc, adapter, this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::iphone::PrimarySwapChain::PrimarySwapChain");
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

    DoneForContext ();

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
  return impl->adapter->GetOutput (0);
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
    exception.touch ("render::low_level::opengl::iphone::PrimarySwapChain::Present");

    throw;
  }
}

/*
    Установка / взятие состояния full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  if (state)
    throw xtl::format_not_supported_exception ("render::low_level::opengl::iphone::PrimarySwapChain::SetFullscreenState",
                                               "Changing fullscreen state not supported");
}

bool PrimarySwapChain::GetFullscreenState ()
{
  return false;
}

/*
   Инициализация связи с контекстом/закрытие связи
*/

void PrimarySwapChain::InitializeForContext (EAGLContext* eagl_context, Context* context)
{
  impl->InitializeForContext (eagl_context, context);
}

void PrimarySwapChain::DoneForContext ()
{
  impl->DoneForContext ();
}

/*
   Получение идентификатора буфера
*/

size_t PrimarySwapChain::GetFrameBufferId ()
{
  if (!impl->frame_buffer)
    throw xtl::format_operation_exception ("render::low_level::opengl::iphone::PrimarySwapChain::GetFrameBufferId", "Can't get frame buffer before initializing swap chain");

  return impl->frame_buffer;
}
