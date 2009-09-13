#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Утилиты
*/

namespace
{

#ifndef OPENGL_ES_SUPPORT

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
      throw xtl::format_not_supported_exception (source, "Unsupported %s=%s", param, get_name (format));
    default:
      throw xtl::make_argument_exception (source, param, format);
  }
}

#else

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
    case PixelFormat_D16:     return GL_DEPTH_COMPONENT16_OES;
    case PixelFormat_D24X8:   return GL_DEPTH_COMPONENT24_OES;
    case PixelFormat_D24S8:   return GL_DEPTH24_STENCIL8_OES;
    case PixelFormat_S8:      return GL_STENCIL_INDEX8_OES;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      throw xtl::format_not_supported_exception (source, "Unsupported %s=%s", param, get_name (format));
    default:
      throw xtl::make_argument_exception (source, param, format);
  }
}

#endif

}

/*
    Конструктор / деструктор
*/

FboRenderBuffer::FboRenderBuffer (const FrameBufferManagerPtr& manager, const TextureDesc& desc)
  : RenderBuffer (manager->GetContextManager (), desc),
    frame_buffer_manager (manager),
    render_buffer_id (0),
    frame_buffer_id (0)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboRenderBuffer::FboRenderBuffer";
  
    //выбор текущего контекста

  MakeContextCurrent ();
  
    //проверка наличия необходимого расширения
    
  const ContextCaps& caps = GetCaps ();
    
  if (!caps.has_ext_framebuffer_object)
    throw xtl::format_not_supported_exception (METHOD_NAME, "GL_EXT_framebuffer_object not supported");
  
    //преобразование формата буфера рендеринга
  
  GLenum internal_format = get_render_buffer_format (desc.format, METHOD_NAME, "desc.format");
  
  if (desc.format == PixelFormat_D24S8)
  {
    if (!GetCaps ().has_ext_packed_depth_stencil)
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported render buffer desc.format=%s (GL_EXT_packed_depth_stencil not supported)", get_name (desc.format));
  }
  
  try
  {  
      //создание буфера рендеринга
    
    caps.glGenRenderbuffers_fn (1, &render_buffer_id);

    if (!render_buffer_id)
      RaiseError (METHOD_NAME);

    caps.glBindRenderbuffer_fn    (GL_RENDERBUFFER, render_buffer_id);
    caps.glRenderbufferStorage_fn (GL_RENDERBUFFER, internal_format, desc.width, desc.height);

      //проверка ошибок

    CheckErrors (METHOD_NAME);
  }
  catch (...)
  {
    if (render_buffer_id) GetCaps ().glDeleteRenderbuffers_fn (1, &render_buffer_id);
  }
}

FboRenderBuffer::~FboRenderBuffer ()
{
  try
  {
    MakeContextCurrent ();
    
    const ContextCaps& caps = GetCaps ();

    caps.glDeleteRenderbuffers_fn (1, &render_buffer_id);

    if (frame_buffer_id)
      caps.glDeleteFramebuffers_fn (1, &frame_buffer_id);

    CheckErrors ("");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FboRenderBuffer::~FboRenderBuffer");
    
    LogPrintf ("%s", exception.what ());
  }  
  catch (std::exception& exception)
  {
    LogPrintf ("%s", exception.what ());
  }
  catch (...)
  {
    //подавляем все исключения
  }  
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
  
  const ContextCaps& caps = GetCaps ();
  
  size_t current_id = GetContextCacheValue (CacheEntry_FrameBufferId);

    //создание буфера кадра
    
  caps.glGenFramebuffers_fn (1, &frame_buffer_id);
  
  if (!frame_buffer_id)
    RaiseError (METHOD_NAME);
    
  try
  {
    SetContextCacheValue (CacheEntry_FrameBufferId, 0); //for exceptions
    
      //установка текущего буфера кадра

    caps.glBindFramebuffer_fn (GL_FRAMEBUFFER, frame_buffer_id);

      //связывание буфера кадра с буфером рендеринга    

    switch (GetTargetType ())
    {
      case RenderTargetType_Color:
        caps.glFramebufferRenderbuffer_fn (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buffer_id);
        break;
      case RenderTargetType_DepthStencil:
        caps.glFramebufferRenderbuffer_fn (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer_id);

        if (GetDesc ().format == PixelFormat_D24X8)
          caps.glFramebufferRenderbuffer_fn (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_id);

        break;
      default:
        break;
    }
    
#ifndef OPENGL_ES_SUPPORT

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
      default:
        break;
    }
    
#endif
    
      //проверка состояния буфера кадра
    
    GLenum status = (GLenum)caps.glCheckFramebufferStatus_fn (GL_FRAMEBUFFER);

    check_frame_buffer_status (METHOD_NAME, status);

      //проверка ошибок

    CheckErrors (METHOD_NAME);    

      //обновление текущего буфера

    SetContextCacheValue (CacheEntry_FrameBufferId, GetId ());

    return frame_buffer_id;
  }
  catch (...)
  {
    GetCaps ().glDeleteFramebuffers_fn (1, &frame_buffer_id);
    
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
    frame_buffer_manager->SetFrameBuffer (GetFrameBufferId (), GetId ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FboRenderBuffer::Bind");

    throw;
  }
}
