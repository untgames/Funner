#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Утилиты
*/

namespace
{

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

}

/*
    Конструктор / деструктор
*/

FboRenderBuffer::FboRenderBuffer (const FrameBufferManager& manager, const TextureDesc& desc)
  : RenderBuffer (manager.GetContextManager (), desc),
    frame_buffer_manager (manager),
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
  
  GLenum internal_format = get_render_buffer_format (desc.format, METHOD_NAME, "desc.format");
  
  if (desc.format == PixelFormat_D24S8)
  {
    static Extension EXT_packed_depth_stencil = "GL_EXT_packed_depth_stencil";
      
    if (!IsSupported (EXT_packed_depth_stencil))
      RaiseNotSupported (METHOD_NAME, "Unsupported render buffer desc.format=%s (GL_EXT_packed_depth_stencil not supported)", get_name (desc.format));
  }
  
  try
  {  
      //создание буфера рендеринга
    
    glGenRenderbuffersEXT (1, &render_buffer_id);

    if (!render_buffer_id)
      RaiseError (METHOD_NAME);

    glBindRenderbufferEXT    (GL_RENDERBUFFER_EXT, render_buffer_id);
    glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT, internal_format, desc.width, desc.height);

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

    if (frame_buffer_id)
      glDeleteFramebuffersEXT (1, &frame_buffer_id);

    CheckErrors ("");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FboRenderBuffer::~FboRenderBuffer");
    
    LogPrintf ("%s", exception.Message ());
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

        if (GetDesc ().format == PixelFormat_D24X8)
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
    frame_buffer_manager.SetFrameBuffer (GetFrameBufferId (), GetId ());
    
    switch (GetTargetType ())
    {
      case RenderTargetType_Color:
        frame_buffer_manager.SetFrameBufferActivity (true, false);
        break;
      case RenderTargetType_DepthStencil:
        frame_buffer_manager.SetFrameBufferActivity (false, true);
        break;
      default:
        frame_buffer_manager.SetFrameBufferActivity (false, false);
        break;
    }
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FboRenderBuffer::Bind");

    throw;
  }
}
