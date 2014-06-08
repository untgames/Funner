#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

FboFrameBuffer::FboFrameBuffer (const FrameBufferManagerPtr& manager, View* color_view, View* depth_stencil_view)
  : ContextObject (manager->GetContextManager ()),
    frame_buffer_manager (manager),
    id (0),
    is_color_buffer_active (false)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboFrameBuffer::FboFrameBuffer";
  
    //выбор текущего контекста

  MakeContextCurrent ();    

    //проверка поддержки необходимого расширения
    
  const ContextCaps& caps = GetCaps ();
    
  if (!caps.has_ext_framebuffer_object)
    throw xtl::format_not_supported_exception (METHOD_NAME, "GL_EXT_framebuffer_object not supported");

    //создание буфера кадра

  caps.glGenFramebuffers_fn (1, &id);

  if (!id)
    RaiseError (METHOD_NAME);

  try
  {    
      //установка текущего буфера кадра в контекст OpenGL
    
    frame_buffer_manager->SetFrameBuffer (id, GetId ());
    
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)
    
      //установка активного буфера вывода и чтения

    if (color_view)
    {
      glDrawBuffer (GL_COLOR_ATTACHMENT0);
      glReadBuffer (GL_COLOR_ATTACHMENT0);
    }
    else
    {
      glDrawBuffer (GL_NONE);
      glReadBuffer (GL_NONE);
    }    

#endif
    
      //инициализация целевых отображений
    
    SetAttachment (RenderTargetType_Color, color_view);
    SetAttachment (RenderTargetType_DepthStencil, depth_stencil_view);
    FinishInitialization ();

      //проверка ошибок

    CheckErrors (METHOD_NAME);    
  }
  catch (...)
  {
    GetCaps ().glDeleteFramebuffers_fn (1, &id);
    throw;
  }    
}

FboFrameBuffer::~FboFrameBuffer ()
{
  try
  {
    if (TryMakeContextCurrent ())
    {
      GetCaps ().glDeleteFramebuffers_fn (1, &id);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FboFrameBuffer::~FboFrameBuffer");

    LogPrintf ("%s\n", exception.what ());
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

  if (!view)
    return;
    
    //установка флага активности целевого буфера

  if (target_type == RenderTargetType_Color)
    is_color_buffer_active = view != 0;
 
    //проверка совместимости хранимой в отображении текстуры и буфера кадра    
    
  ITexture* base_texture = view->GetTexture ();
  
  if (!base_texture)
    throw xtl::format_operation_exception (METHOD_NAME, "Internal error: view with null texture");
  
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

  throw xtl::format_operation_exception (METHOD_NAME, "%s has unknown texture type %s", target_name, view->GetTextureTypeName ());
}

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

void FboFrameBuffer::SetAttachment (GLenum attachment, GLenum textarget, size_t texture_id, const ViewDesc& view_desc)
{
  const ContextCaps& caps = GetCaps ();

  switch (textarget)
  {
    case GL_TEXTURE_1D:
      caps.glFramebufferTexture1D_fn (GL_FRAMEBUFFER, attachment, textarget, texture_id, view_desc.mip_level);
      break;
    case GL_TEXTURE_CUBE_MAP:
      textarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + view_desc.layer;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE_ARB:
      caps.glFramebufferTexture2D_fn (GL_FRAMEBUFFER, attachment, textarget, texture_id, view_desc.mip_level);
      break;
    case GL_TEXTURE_3D_EXT:
      caps.glFramebufferTexture3D_fn (GL_FRAMEBUFFER, attachment, textarget, texture_id, view_desc.mip_level, view_desc.layer);
      break;
    default:
      throw xtl::format_operation_exception ("render::low_level::opengl::FboFrameBuffer::SetAttachment", "Unknown textarget=0x%04x", textarget);
  }
}

#else

void FboFrameBuffer::SetAttachment (GLenum attachment, GLenum textarget, size_t texture_id, const ViewDesc& view_desc)
{
  const ContextCaps& caps = GetCaps ();

  switch (textarget)
  {
    case GL_TEXTURE_CUBE_MAP:
      textarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + view_desc.layer;
    case GL_TEXTURE_2D:
      caps.glFramebufferTexture2D_fn (GL_FRAMEBUFFER, attachment, textarget, texture_id, view_desc.mip_level);
      break;
    default:
      throw xtl::format_operation_exception ("render::low_level::opengl::FboFrameBuffer::SetAttachment", "Unknown textarget=0x%04x", textarget);
  }
}

#endif

void FboFrameBuffer::SetAttachment (RenderTargetType target_type, IRenderTargetTexture* texture, const ViewDesc& view_desc, const TextureDesc& texture_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboFrameBuffer::SetAttachment(RenderTargetType,IRenderTargetTexture*,const ViewDesc&)";

  if (view_desc.mip_level)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported mip_level=0 (incompatible with GL_EXT_framebuffer_object)");

  RenderTargetTextureDesc render_target_desc;

  texture->GetDesc (render_target_desc);

  switch (target_type)
  {
    case RenderTargetType_Color:
      SetAttachment (GL_COLOR_ATTACHMENT0, render_target_desc.target, render_target_desc.id, view_desc);
      break;
    case RenderTargetType_DepthStencil:
      SetAttachment (GL_DEPTH_ATTACHMENT, render_target_desc.target, render_target_desc.id, view_desc);

      switch (texture_desc.format)
      {
        case PixelFormat_D24S8:
          SetAttachment (GL_STENCIL_ATTACHMENT, render_target_desc.target, render_target_desc.id, view_desc);
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
  const ContextCaps& caps = GetCaps ();

  size_t render_buffer_id = render_buffer->GetRenderBufferId ();

  switch (target_type)
  {
    case RenderTargetType_Color:
      caps.glFramebufferRenderbuffer_fn (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buffer_id);
      break;
    case RenderTargetType_DepthStencil:
    {
      TextureDesc desc;
      
      render_buffer->GetDesc (desc);
      
      caps.glFramebufferRenderbuffer_fn (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer_id);

      switch (desc.format)
      {
        case PixelFormat_D24S8:
          caps.glFramebufferRenderbuffer_fn (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_id);
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
  
  GLenum status = (GLenum)GetCaps ().glCheckFramebufferStatus_fn (GL_FRAMEBUFFER);

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
    frame_buffer_manager->SetFrameBuffer (id, GetId ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FboFrameBuffer::Bind");
    throw;
  }
}
