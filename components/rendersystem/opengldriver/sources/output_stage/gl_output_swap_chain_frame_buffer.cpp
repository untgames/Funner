#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

SwapChainFrameBuffer::SwapChainFrameBuffer
 (const ContextManager&        manager,
  SwapChainColorBuffer*        in_color_buffer,
  SwapChainDepthStencilBuffer* in_depth_stencil_buffer)
    : ContextObject (manager),
      color_buffer (in_color_buffer),
      depth_stencil_buffer (in_depth_stencil_buffer),
      color_buffer_state (true),
      depth_stencil_buffer_state (true)
{
  memset (&render_target_desc, 0, sizeof (render_target_desc));
  memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));
}

/*
    Установка буфера в контекст OpenGL
*/

void SwapChainFrameBuffer::Bind (bool& out_color_buffer_state, bool& out_depth_stencil_buffer_state)
{
  size_t      context_id  = depth_stencil_buffer ? depth_stencil_buffer->GetContextId () : 0;
  ISwapChain* swap_chain  = color_buffer->GetSwapChain ();
  GLenum      buffer_type = color_buffer_state ? color_buffer->GetBufferType () : GL_NONE;
  
    //установка активного контекста

  GetContextManager ().SetContext (context_id, swap_chain, swap_chain);
  
    //установка текущего контекста OpenGL

  MakeContextCurrent ();
  
    //установка текущего буфера чтения и отрисовки    

  glReadBuffer (buffer_type);
  glDrawBuffer (buffer_type);
  
    //проверка ошибок

  CheckErrors ("render::low_level::opengl::SwapChainFrameBuffer::Bind");
  
    //установка флагов состояния буферов отрисовки
    
  out_color_buffer_state         = color_buffer_state;
  out_depth_stencil_buffer_state = depth_stencil_buffer_state;
}

/*
    Разрешение / запрещение буферов
*/

void SwapChainFrameBuffer::SetBuffersState (bool in_color_buffer_state, bool in_depth_stencil_buffer_state)
{
  color_buffer_state         = in_color_buffer_state;
  depth_stencil_buffer_state = in_depth_stencil_buffer_state;
}

/*
    Работа с текстурами
*/

void SwapChainFrameBuffer::SetRenderTargets
 (IBindableTexture* in_render_target_texture,
  const ViewDesc*   in_render_target_desc,
  IBindableTexture* in_depth_stencil_texture,
  const ViewDesc*   in_depth_stencil_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::SetRenderTargets";

  if (in_render_target_texture)
  {
    TextureDesc desc;
    
    in_render_target_texture->GetDesc (desc);
    
    switch (desc.format)
    {
      case PixelFormat_RGB8:
      case PixelFormat_RGBA8:
      case PixelFormat_L8:
      case PixelFormat_A8:
      case PixelFormat_LA8:
      case PixelFormat_DXT1:
      case PixelFormat_DXT3:
      case PixelFormat_DXT5:
        break;
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_S8:
        RaiseNotSupported (METHOD_NAME, "Can not bind texture with format %s as render-target", get_name (desc.format));
        break;
      default:
        RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
        break;
    }
  }
  
  if (in_depth_stencil_texture)
  {
    TextureDesc desc;
    
    in_depth_stencil_texture->GetDesc (desc);
    
    switch (desc.format)
    {
      case PixelFormat_RGB8:
      case PixelFormat_RGBA8:
      case PixelFormat_L8:
      case PixelFormat_A8:
      case PixelFormat_LA8:
      case PixelFormat_DXT1:
      case PixelFormat_DXT3:
      case PixelFormat_DXT5:
        RaiseNotSupported (METHOD_NAME, "Can not bind texture with format %s as depth-stencil", get_name (desc.format));
        break;
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_S8:
        break;
      default:
        RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
        break;
    }
  }  

  render_target_texture = in_render_target_texture;
  depth_stencil_texture = in_depth_stencil_texture;    
  
  if (in_render_target_desc)
  {
    render_target_desc = *in_render_target_desc;
  }
  else
  {
    memset (&render_target_desc, 0, sizeof (render_target_desc));
  }
  
  if (in_depth_stencil_desc)
  {
    depth_stencil_desc = *in_depth_stencil_desc;
  }
  else
  {
    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));
  }
}

namespace
{

void copy_image (size_t width, size_t height, const BindableTextureDesc& texture_desc, const ViewDesc& view_desc)
{
  glBindTexture (texture_desc.target, texture_desc.id);  
  
  switch (texture_desc.target)
  {
    case GL_TEXTURE_1D:
      glCopyTexSubImage1D (GL_TEXTURE_1D, view_desc.mip_level, 0, 0, 0, width);
      break;
    case GL_TEXTURE_2D:
      glCopyTexSubImage2D (GL_TEXTURE_2D, view_desc.mip_level, 0, 0, 0, 0, width, height);
      break;
    default:
      break;
  }
}

}

void SwapChainFrameBuffer::UpdateRenderTargets ()
{
    //данный метод вызывается в установленном контексте, потому не требует вызова MakeConextCurrent
    
    //получение информации о цепочке обмена
    
  SwapChainDesc swap_chain_desc;

  color_buffer->GetSwapChain ()->GetDesc (swap_chain_desc);
  
  size_t width  = swap_chain_desc.frame_buffer.width,
         height = swap_chain_desc.frame_buffer.height;

    //копирование текстуры из буфера цвета
    
  if (render_target_texture)
  {
    BindableTextureDesc bindable_desc;
    TextureDesc         texture_desc;

    render_target_texture->GetDesc (bindable_desc);
    render_target_texture->GetDesc (texture_desc);

    copy_image (width < texture_desc.width ? width : texture_desc.width,
                height < texture_desc.height ? height : texture_desc.height, bindable_desc, render_target_desc);
  }
  
    //копирование текстуры из буфера глубины
    
  if (depth_stencil_texture)
  {
    BindableTextureDesc bindable_desc;
    TextureDesc         texture_desc;

    depth_stencil_texture->GetDesc (bindable_desc);
    depth_stencil_texture->GetDesc (texture_desc);

    copy_image (width < texture_desc.width ? width : texture_desc.width,
                height < texture_desc.height ? height : texture_desc.height, bindable_desc, depth_stencil_desc);
  }
  
  CheckErrors ("render::low_level::opengl::SwapChainFrameBuffer::UpdateRenderTargets");
}
