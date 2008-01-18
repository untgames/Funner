#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
===================================================================================================
    Утилиты
===================================================================================================
*/

namespace
{

//определение типа цели рендеринга
RenderTargetType get_render_target_type (PixelFormat format, const char* source, const char* param)
{
  switch (format)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:   
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:     return RenderTargetType_Color;
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_S8:      return RenderTargetType_DepthStencil;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      RaiseNotSupported (source, "Unsupported %p=%s", param, get_name (format));
      return RenderTargetType_Color;
    default:
      RaiseInvalidArgument (source, param, format);
      return RenderTargetType_Color;
  }
}

//определение типа буфера рендеринга
GLenum get_render_buffer_format (PixelFormat format, const char* source, const char* param)
{
  switch (format)
  {
    case PixelFormat_RGB8:    return GL_RGB;
    case PixelFormat_RGBA8:   return GL_RGBA;
    case PixelFormat_L8:      return GL_LUMINANCE;
    case PixelFormat_A8:      return GL_ALPHA;
    case PixelFormat_LA8:     return GL_LUMINANCE_ALPHA;
    case PixelFormat_D16:     return GL_DEPTH_COMPONENT16;
    case PixelFormat_D24X8:   return GL_DEPTH_COMPONENT24;
    case PixelFormat_D24S8:   return GL_DEPTH24_STENCIL8_EXT;
    case PixelFormat_S8:      return GL_STENCIL_INDEX8_EXT;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      RaiseNotSupported (source, "Unsupported %p=%s", param, get_name (format));
      return 0;
    default:
      RaiseInvalidArgument (source, param, format);
      return 0;
  }
}

//проверка состояния текущего буфера кадра
void check_frame_buffer_status (const char* source, GLenum status)
{
  switch (status)
  {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: incomplete attachment");
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: unsupported framebuffer format");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: missing attachment");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: attached images must have same dimensions");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: attached images must have same format");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: missing draw buffer");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: missing read buffer");
      break;
    default:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: 0x%04x", status);
      break;
  }
}

}

/*
===================================================================================================
    FboRenderBuffer
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

FboRenderBuffer::FboRenderBuffer (const ContextManager& manager, PixelFormat format, size_t width, size_t height)
  : RenderBuffer (manager, get_render_target_type (format, "render::low_level::opengl::FboRenderBuffer::FboRenderBuffer", "format")),
    render_buffer_id (0),
    frame_buffer_id (0)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboRenderBuffer::FboRenderBuffer";
  
    //выбор текущего контекста

  MakeContextCurrent ();
  
    //проверка наличия необходимого расширения
    
  static Extension EXT_framebuffer_object = "GL_EXT_framebuffer_object";
  
  if (!IsSupported (EXT_framebuffer_object))
    RaiseNotSupported (METHOD_NAME, "GL_EXT_framebuffer_object not supported");
  
    //преобразование формата буфера рендеринга
  
  GLenum internal_format = get_render_buffer_format (format, METHOD_NAME, "desc.format");
  
  if (format == PixelFormat_D24S8)
  {
    static Extension EXT_packed_depth_stencil = "GL_EXT_packed_depth_stencil";
      
    if (!IsSupported (EXT_packed_depth_stencil))
      RaiseNotSupported (METHOD_NAME, "Unsupported render buffer format=%s (GL_EXT_packed_depth_stencil not supported)", get_name (format));
  }
  
  try
  {  
      //создание буфера рендеринга
    
    glGenRenderbuffersEXT (1, &render_buffer_id);

    if (!render_buffer_id)
      RaiseError (METHOD_NAME);

    glBindRenderbufferEXT    (GL_RENDERBUFFER_EXT, render_buffer_id);
    glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT, internal_format, width, height);

      //заполнение дескриптора

    memset (&desc, 0, sizeof (desc));

    desc.dimension            = TextureDimension_2D;
    desc.width                = width;
    desc.height               = height;
    desc.layers               = 1;
    desc.format               = format;
    desc.generate_mips_enable = false;
    desc.access_flags         = AccessFlag_Read | AccessFlag_Write;
    desc.bind_flags           = GetTargetType () == RenderTargetType_Color ? BindFlag_RenderTarget : BindFlag_DepthStencil;
    desc.usage_mode           = UsageMode_Static;

      //проверка ошибок

    CheckErrors (METHOD_NAME);
  }
  catch (...)
  {
    if (render_buffer_id) glDeleteRenderbuffersEXT (1, &render_buffer_id);
  }
}

FboRenderBuffer::~FboRenderBuffer ()
{
  try
  {
    MakeContextCurrent ();

    glDeleteRenderbuffersEXT (1, &render_buffer_id);
    glDeleteFramebuffersEXT  (1, &frame_buffer_id);

    CheckErrors ("");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FboRenderBuffer::~FboRenderBuffer");
    
    LogPrintf ("%s\n", exception.Message ());
  }  
  catch (std::exception& exception)
  {
    LogPrintf ("%s\n", exception.what ());
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Получение дескриптора текстуры
*/

void FboRenderBuffer::GetDesc (TextureDesc& out_desc)
{
  out_desc = desc;
}

/*
    Получение OpenGL идентификатора буфера кадра для возможности ручной записи / чтения данных
*/

size_t FboRenderBuffer::GetFrameBufferId ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboRenderBuffer::GetFrameBufferId";

    //если буфер создан - возвращаем его

  if (frame_buffer_id)
    return frame_buffer_id;    
    
    //выбор текущего контекста
    
  MakeContextCurrent ();

    //создание буфера кадра
    
  glGenFramebuffersEXT (1, &frame_buffer_id);
  
  if (!frame_buffer_id)
    RaiseError (METHOD_NAME);
    
  try
  {
      //установка текущего буфера кадра
    
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, frame_buffer_id);    
    
      //связывание буфера кадра с буфером рендеринга    
    
    switch (GetTargetType ())
    {
      case RenderTargetType_Color:
        glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, render_buffer_id);
        break;
      case RenderTargetType_DepthStencil:
        glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, render_buffer_id);

        if (desc.format == PixelFormat_D24X8)
          glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, render_buffer_id);

        break;
    }

      //настройка буферов рисования и чтения      
    
    switch (GetTargetType ())
    {
      case RenderTargetType_Color:
        glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
        glReadBuffer (GL_COLOR_ATTACHMENT0_EXT);
        break;
      case RenderTargetType_DepthStencil:
        glDrawBuffer (GL_NONE);
        glReadBuffer (GL_NONE);
        break;
    }    
    
      //проверка состояния буфера кадра
    
    GLenum status = (GLenum)glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);    
    
    check_frame_buffer_status (METHOD_NAME, status);

      //проверка ошибок

    CheckErrors (METHOD_NAME);
    
    return frame_buffer_id;
  }
  catch (...)
  {
    glDeleteFramebuffersEXT (1, &frame_buffer_id);
    
    frame_buffer_id = 0;
    
    throw;
  }
}

/*
    Установка буфера в контекст OpenGL
*/

void FboRenderBuffer::Bind ()
{
  try
  {
      //выбор текущего контекста

    MakeContextCurrent ();

      //установка буфера кадра

    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, GetFrameBufferId ());

      //проверка ошибок

    CheckErrors ("");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FboRenderBuffer::Bind");
    
    throw;
  }
}

/*
===================================================================================================
    FboFrameBufferBase
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

FboFrameBufferBase::FboFrameBufferBase (const ContextManager& manager, bool in_color_buffer_state, bool in_depth_stencil_buffer_state)
  : ContextObject (manager),
    id (0),
    color_buffer_state (in_color_buffer_state),
    depth_stencil_buffer_state (in_depth_stencil_buffer_state)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboFrameBufferBase::FboFrameBufferBase";

    //выбор текущего контекста

  MakeContextCurrent ();
  
    //проверка поддержки необходимого расширения
    
  static Extension EXT_framebuffer_object = "GL_EXT_framebuffer_object";
  
  if (!IsSupported (EXT_framebuffer_object))
    RaiseNotSupported (METHOD_NAME, "GL_EXT_framebuffer_object not supported");

    //создание и выбор буфера кадра

  glGenFramebuffersEXT (1, &id);

  if (!id)
    RaiseError (METHOD_NAME);

  glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, id);
  
    //установка активного буфера вывода и чтения
  
  if (color_buffer_state)
  {
    glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
    glReadBuffer (GL_COLOR_ATTACHMENT0_EXT);
  }
  else
  {
    glDrawBuffer (GL_NONE);
    glReadBuffer (GL_NONE);
  }

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}

FboFrameBufferBase::~FboFrameBufferBase ()
{
  try
  {
    MakeContextCurrent ();

    glDeleteFramebuffersEXT (1, &id);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FboFrameBufferBase::~FboFrameBufferBase");

    LogPrintf ("%s\n", exception.Message ());
  }  
  catch (std::exception& exception)
  {
    LogPrintf ("%s\n", exception.what ());
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Выбор буфера в контекст OpenGL
*/

void FboFrameBufferBase::Bind (bool& out_color_buffer_state, bool& out_depth_stencil_buffer_state)
{
  MakeContextCurrent ();
  
  glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, id);
  
  out_color_buffer_state         = color_buffer_state;
  out_depth_stencil_buffer_state = depth_stencil_buffer_state;
  
  CheckErrors ("render::low_level::opengl::FboFrameBufferBase::~FboFrameBufferBase");
}

/*
===================================================================================================
    FboFrameBuffer
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

FboFrameBuffer::FboFrameBuffer (const ContextManager& manager, NullView, FboRenderBuffer* depth_stencil_buffer)
  : FboFrameBufferBase (manager, false, true)
{
  SetAttachment (RenderTargetType_DepthStencil, depth_stencil_buffer);
  FinishInitialization ();
}

FboFrameBuffer::FboFrameBuffer
 (const ContextManager& manager,
  NullView,
  IBindableTexture*     depth_stencil_texture,
  const ViewDesc&       depth_stencil_desc)
    : FboFrameBufferBase (manager, false, true)
{
  SetAttachment (RenderTargetType_DepthStencil, depth_stencil_texture, depth_stencil_desc);
  FinishInitialization ();
}

FboFrameBuffer::FboFrameBuffer (const ContextManager& manager, FboRenderBuffer* color_buffer, NullView)
  : FboFrameBufferBase (manager, true, false)
{
  SetAttachment (RenderTargetType_Color, color_buffer);
  FinishInitialization ();
}

FboFrameBuffer::FboFrameBuffer (const ContextManager& manager, FboRenderBuffer* color_buffer, FboRenderBuffer* depth_stencil_buffer)
  : FboFrameBufferBase (manager, true, true)
{
  SetAttachment (RenderTargetType_Color, color_buffer);
  SetAttachment (RenderTargetType_DepthStencil, depth_stencil_buffer);
  FinishInitialization ();
}

FboFrameBuffer::FboFrameBuffer
 (const ContextManager& manager,
  FboRenderBuffer*      color_buffer,
  IBindableTexture*     depth_stencil_texture,
  const ViewDesc&       depth_stencil_desc)
    : FboFrameBufferBase (manager, true, true)
{
  SetAttachment (RenderTargetType_Color, color_buffer);
  SetAttachment (RenderTargetType_DepthStencil, depth_stencil_texture, depth_stencil_desc);
  FinishInitialization ();
}

FboFrameBuffer::FboFrameBuffer
 (const ContextManager& manager,
  IBindableTexture*     render_target_texture,
  const ViewDesc&       render_target_desc,
  NullView)
    : FboFrameBufferBase (manager, true, false)
{
  SetAttachment (RenderTargetType_Color, render_target_texture, render_target_desc);
  FinishInitialization ();
}

FboFrameBuffer::FboFrameBuffer
 (const ContextManager& manager,
  IBindableTexture*     render_target_texture,
  const ViewDesc&       render_target_desc,
  FboRenderBuffer*      depth_stencil_buffer)
    : FboFrameBufferBase (manager, true, true)
{
  SetAttachment (RenderTargetType_Color, render_target_texture, render_target_desc);
  SetAttachment (RenderTargetType_DepthStencil, depth_stencil_buffer);
  FinishInitialization ();
}

FboFrameBuffer::FboFrameBuffer
 (const ContextManager& manager,
  IBindableTexture*     render_target_texture,
  const ViewDesc&       render_target_desc,
  IBindableTexture*     depth_stencil_texture,
  const ViewDesc&       depth_stencil_desc)
    : FboFrameBufferBase (manager, true, true)
{
  SetAttachment (RenderTargetType_Color, render_target_texture, render_target_desc);
  SetAttachment (RenderTargetType_DepthStencil, depth_stencil_texture, depth_stencil_desc);
  FinishInitialization ();
}

/*
    Присоединение буфера рендеринга к текущему буферу кадра
*/

void FboFrameBuffer::SetAttachment (GLenum attachment, GLenum textarget, size_t texture_id, const ViewDesc& view_desc)
{
  switch (textarget)
  {
    case GL_TEXTURE_1D:
      glFramebufferTexture1DEXT (GL_FRAMEBUFFER_EXT, attachment, textarget, texture_id, view_desc.mip_level);
      break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
      glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, attachment, textarget, texture_id, view_desc.mip_level);
      break;
    case GL_TEXTURE_3D_EXT:
      glFramebufferTexture3DEXT (GL_FRAMEBUFFER_EXT, attachment, textarget, texture_id, view_desc.mip_level, view_desc.layer);
      break;
    default:
      RaiseInvalidOperation ("render::low_level::opengl::FboFrameBuffer::SetAttachment", "Unknown textarget=0x%04x", textarget);
      break;
  }
}

void FboFrameBuffer::SetAttachment (RenderTargetType target_type, IBindableTexture* texture, const ViewDesc& view_desc)
{
  TextureDesc         desc;
  BindableTextureDesc bind_desc;

  memset (&desc, 0, sizeof (desc));
  memset (&bind_desc, 0, sizeof (bind_desc));

  texture->GetDesc (desc);
  texture->GetDesc (bind_desc);

  switch (target_type)
  {
    case RenderTargetType_Color:
      SetAttachment (GL_COLOR_ATTACHMENT0_EXT, bind_desc.target, bind_desc.id, view_desc);
      break;
    case RenderTargetType_DepthStencil:
      SetAttachment (GL_DEPTH_ATTACHMENT_EXT, bind_desc.target, bind_desc.id, view_desc);
      SetAttachment (GL_STENCIL_ATTACHMENT_EXT, bind_desc.target, bind_desc.id, view_desc);
      break;
    default:
      break;
  }
}

void FboFrameBuffer::SetAttachment (RenderTargetType target_type, FboRenderBuffer* render_buffer)
{
  size_t render_buffer_id = render_buffer->GetRenderBufferId ();

  switch (target_type)
  {
    case RenderTargetType_Color:
      glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, render_buffer_id);
      break;
    case RenderTargetType_DepthStencil:
    {
      TextureDesc desc;
      
      memset (&desc, 0, sizeof (desc));
      
      render_buffer->GetDesc (desc);
      
      glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, render_buffer_id);

      if (desc.format == PixelFormat_D24X8)
        glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, render_buffer_id);

      break;
    }
    default:
      break;
  }
}

/*
    Завершение инициализации
*/

void FboFrameBuffer::FinishInitialization ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboFrameBuffer::FboFrameBuffer";

    //проверка состояния буфера кадра
  
  GLenum status = (GLenum)glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);    

  check_frame_buffer_status (METHOD_NAME, status);

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}
