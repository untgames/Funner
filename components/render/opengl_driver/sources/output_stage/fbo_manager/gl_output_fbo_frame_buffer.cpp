#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

FboFrameBuffer::FboFrameBuffer (const FrameBufferManager& manager, View* color_view, View* depth_stencil_view)
  : ContextObject (manager.GetContextManager ()),
    frame_buffer_manager (manager),
    id (0)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboFrameBuffer::FboFrameBuffer";
  
    //сброс флагов активности целевых буферов
    
  memset (is_active, 0, sizeof is_active);

    //выбор текущего контекста

  MakeContextCurrent ();

    //проверка поддержки необходимого расширения
    
  if (!GetCaps ().has_ext_framebuffer_object)
    raise_not_supported (METHOD_NAME, "GL_EXT_framebuffer_object not supported");

    //создание буфера кадра

  glGenFramebuffersEXT (1, &id);

  if (!id)
    RaiseError (METHOD_NAME);

  try
  {    
      //установка текущего буфера кадра в контекст OpenGL
    
    frame_buffer_manager.SetFrameBuffer (id, GetId ());
    
      //установка активного буфера вывода и чтения

    if (color_view)
    {
      glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
      glReadBuffer (GL_COLOR_ATTACHMENT0_EXT);
    }
    else
    {
      glDrawBuffer (GL_NONE);
      glReadBuffer (GL_NONE);
    }    
    
      //инициализация целевых отображений
    
    SetAttachment (RenderTargetType_Color, color_view);
    SetAttachment (RenderTargetType_DepthStencil, depth_stencil_view);
    FinishInitialization ();

      //проверка ошибок

    CheckErrors (METHOD_NAME);    
  }
  catch (...)
  {
    glDeleteFramebuffersEXT (1, &id);
    throw;
  }    
}

FboFrameBuffer::~FboFrameBuffer ()
{
  try
  {
    MakeContextCurrent ();

    glDeleteFramebuffersEXT (1, &id);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FboFrameBuffer::~FboFrameBuffer");

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
    Установка целевых отображений
*/

void FboFrameBuffer::SetAttachment (RenderTargetType target_type, View* view)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboFrameBuffer::SetAttachment";

    //установка флага активности целевого буфера

  is_active [target_type] = view != 0;  

  if (!view)
    return;
    
    //проверка совместимости хранимой в отображении текстуры и буфера кадра    
    
  ITexture* base_texture = view->GetTexture ();
  
  if (!base_texture)
    raise_invalid_operation (METHOD_NAME, "Internal error: view with null texture");
  
    //обработка случая рендеринга в текстуру
  
  if (IRenderTargetTexture* texture = dynamic_cast<IRenderTargetTexture*> (base_texture))
  {
    ViewDesc    view_desc;
    TextureDesc texture_desc;

    view->GetDesc         (view_desc);
    base_texture->GetDesc (texture_desc);
    SetAttachment         (target_type, texture, view_desc, texture_desc);

    return;
  }
  
    //обработка случая рендеринга в render-buffer
    
  if (FboRenderBuffer* render_buffer = dynamic_cast<FboRenderBuffer*> (base_texture))
  {
    SetAttachment (target_type, render_buffer);

    return;
  }
  
    //если целевая текстура имеет неизвестный тип - создание буфера кадра невозможно
    
  const char* target_name;

  switch (target_type)
  {
    case RenderTargetType_Color:        target_name = "Color view"; break;
    case RenderTargetType_DepthStencil: target_name = "Depth-stencil view"; break;
    default:                            target_name = "Unknown target view"; break;
  }

  raise_invalid_operation (METHOD_NAME, "%s has unknown texture type %s", target_name, view->GetTextureTypeName ());
}

void FboFrameBuffer::SetAttachment (GLenum attachment, GLenum textarget, size_t texture_id, const ViewDesc& view_desc)
{
  switch (textarget)
  {
    case GL_TEXTURE_1D:
      glFramebufferTexture1DEXT (GL_FRAMEBUFFER_EXT, attachment, textarget, texture_id, view_desc.mip_level);
      break;
    case GL_TEXTURE_CUBE_MAP:
      textarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + view_desc.layer;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE_ARB:
      glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, attachment, textarget, texture_id, view_desc.mip_level);
      break;
    case GL_TEXTURE_3D_EXT:
      glFramebufferTexture3DEXT (GL_FRAMEBUFFER_EXT, attachment, textarget, texture_id, view_desc.mip_level, view_desc.layer);
      break;
    default:
      raise_invalid_operation ("render::low_level::opengl::FboFrameBuffer::SetAttachment", "Unknown textarget=0x%04x", textarget);
      break;
  }
}

void FboFrameBuffer::SetAttachment (RenderTargetType target_type, IRenderTargetTexture* texture, const ViewDesc& view_desc, const TextureDesc& texture_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboFrameBuffer::SetAttachment(RenderTargetType,IRenderTargetTexture*,const ViewDesc&)";

  if (view_desc.mip_level)
    raise_not_supported (METHOD_NAME, "Unsupported mip_level=0 (incompatible with GL_EXT_framebuffer_object)");

  RenderTargetTextureDesc render_target_desc;

  texture->GetDesc (render_target_desc);

  switch (target_type)
  {
    case RenderTargetType_Color:
      SetAttachment (GL_COLOR_ATTACHMENT0_EXT, render_target_desc.target, render_target_desc.id, view_desc);
      break;
    case RenderTargetType_DepthStencil:
      SetAttachment (GL_DEPTH_ATTACHMENT_EXT, render_target_desc.target, render_target_desc.id, view_desc);

      switch (texture_desc.format)
      {
        case PixelFormat_D24S8:
          SetAttachment (GL_STENCIL_ATTACHMENT_EXT, render_target_desc.target, render_target_desc.id, view_desc);
          break;
        default:
          break;
      }

      break;
    default:
      break;
  }
  
    //проверка ошибок

  CheckErrors (METHOD_NAME);
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
      
      render_buffer->GetDesc (desc);
      
      glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, render_buffer_id);

      switch (desc.format)
      {
        case PixelFormat_D24S8:
          glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, render_buffer_id);
          break;
        default:
          break;
      }

      break;
    }
    default:
      break;
  }
  
    //проверка ошибок

  CheckErrors ("render::low_level::opengl::FboFrameBuffer::SetAttachment(RenderTargetType,FboRenderBuffer*)");
}

/*
    Завершение инициализации
*/

void FboFrameBuffer::FinishInitialization ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboFrameBuffer::FinishInitialization";

    //проверка состояния буфера кадра
  
  GLenum status = (GLenum)glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);

  check_frame_buffer_status (METHOD_NAME, status);

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}


/*
    Выбор буфера в контекст OpenGL
*/

void FboFrameBuffer::Bind ()
{
  try
  {
    frame_buffer_manager.SetFrameBuffer (id, GetId ());
    frame_buffer_manager.SetFrameBufferActivity (is_active [RenderTargetType_Color], is_active [RenderTargetType_DepthStencil]);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FboFrameBuffer::Bind");
    throw;
  }
}
