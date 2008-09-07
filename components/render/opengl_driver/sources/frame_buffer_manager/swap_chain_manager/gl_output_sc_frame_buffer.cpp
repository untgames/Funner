#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    SwapChainFrameBuffer::RenderTarget
*/

inline SwapChainFrameBuffer::RenderTarget::RenderTarget ()
{
  memset (&view_desc, 0, sizeof view_desc);
  memset (&mip_level_desc, 0, sizeof mip_level_desc);
  memset (&texture_desc, 0, sizeof texture_desc);  
}

/*
    Конструктор / деструктор
*/

SwapChainFrameBuffer::SwapChainFrameBuffer (SwapChainFrameBufferManager& manager, View* color_view, View* depth_stencil_view)
  : ContextObject (manager.GetFrameBufferManager ().GetContextManager ()),
    frame_buffer_manager (manager.GetFrameBufferManager ()),
    has_texture_targets (false)
{
  try
  {
    memset (is_buffer_active, 0, sizeof is_buffer_active);
    memset (&dirty_rect, 0, sizeof dirty_rect);
   
    SetColorView         (color_view);
    SetDepthStencilView  (depth_stencil_view);
    FinishInitialization (manager);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainFrameBuffer::SwapChainFrameBuffer");
    throw;
  }
}

/*
    Инициализация целевых отображений
*/

void SwapChainFrameBuffer::SetColorView (View* view)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::SetColorView";  

    //проверка наличия буфера цвета

  if (!view)
    return;    
  
    //проверка совместимости хранимой в отображении текстуры и буфера кадра
    
  ITexture* base_texture = view->GetTexture ();
  
  if (!base_texture)
    throw xtl::format_operation_exception (METHOD_NAME, "Internal error: view with null texture");
  
    //установка флага активности буфера цвета

  is_buffer_active [RenderTargetType_Color] = true;

    //обработка случая рендеринга в текстуру
    
  if (IRenderTargetTexture* texture = dynamic_cast<IRenderTargetTexture*> (base_texture))
  {
    RenderTarget& render_target = render_targets [RenderTargetType_Color];
    
    render_target.target_texture = texture;    

    view->GetDesc            (render_target.view_desc);
    texture->GetMipLevelDesc (render_target.view_desc.mip_level, render_target.mip_level_desc);
    texture->GetDesc         (render_target.texture_desc);    

    switch (render_target.texture_desc.format)
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
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported color render-target texture format=%s", get_name (render_target.texture_desc.format));
        break;
      default:
        throw xtl::make_argument_exception (METHOD_NAME, "texture_desc.format", render_target.texture_desc.format);
        break;
    }    
    
    has_texture_targets = true;    

    return;
  }
  
    //обработка случая рендеринга в SwapChainColorBuffer
      
  if (SwapChainColorBuffer* buffer = dynamic_cast<SwapChainColorBuffer*> (base_texture))
  {
    color_buffer = buffer;
    
    return;
  }
  
    //если целевая текстура имеет неизвестный тип - создание буфера кадра невозможно

  throw xtl::format_operation_exception (METHOD_NAME, "Color-view texture has unknown type %s", view->GetTextureTypeName ());
}

void SwapChainFrameBuffer::SetDepthStencilView (View* view)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::SetDepthStencilView";

    //проверка наличия буфера попиксельного отсечения

  if (!view)
    return;

    //проверка совместимости хранимой в отображении текстуры и буфера кадра

  ITexture* base_texture = view->GetTexture ();
  
  if (!base_texture)
    throw xtl::format_operation_exception (METHOD_NAME, "Internal error: view with null texture");
  
    //установка флага активности буфера попиксельного отсечения

  is_buffer_active [RenderTargetType_DepthStencil] = true;

    //обработка случая рендеринга в текстуру

  if (IRenderTargetTexture* texture = dynamic_cast<IRenderTargetTexture*> (base_texture))
  {
    RenderTarget& render_target = render_targets [RenderTargetType_DepthStencil];

    render_target.target_texture = texture;

    view->GetDesc            (render_target.view_desc);
    texture->GetMipLevelDesc (render_target.view_desc.mip_level, render_target.mip_level_desc);    
    texture->GetDesc         (render_target.texture_desc);
    
    switch (render_target.texture_desc.format)
    {
      case PixelFormat_RGB8:
      case PixelFormat_RGBA8:
      case PixelFormat_L8:
      case PixelFormat_A8:
      case PixelFormat_LA8:
      case PixelFormat_DXT1:
      case PixelFormat_DXT3:
      case PixelFormat_DXT5:
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported depth-stencil render-target texture format=%s", get_name (render_target.texture_desc.format));
        break;
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_S8:
        break;
      default:
        throw xtl::make_argument_exception (METHOD_NAME, "texture_desc.format", render_target.texture_desc.format);
        break;
    }    
    
    has_texture_targets = true;    

    return;
  }

    //обработка случая рендеринга в SwapChainDepthStencilBuffer

  if (SwapChainDepthStencilBuffer* buffer = dynamic_cast<SwapChainDepthStencilBuffer*> (base_texture))
  {
    depth_stencil_buffer = buffer;

    return;
  }
  
    //обработка случая рендеринга во вспомогательный буфер
    
  if (SwapChainFakeDepthStencilBuffer* buffer = dynamic_cast<SwapChainFakeDepthStencilBuffer*> (base_texture))
  {
    fake_depth_stencil_buffer = buffer;

    return;
  }

    //если целевая текстура имеет неизвестный тип - создание буфера кадра невозможно

  throw xtl::format_operation_exception (METHOD_NAME, "Depth-stencil-view texture has unknown type %s", view->GetTextureTypeName ());
}

void SwapChainFrameBuffer::FinishInitialization (SwapChainFrameBufferManager& manager)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::FinishInitialization";

    //если оба буфера проинициализированы - проверка совместимости

  if (color_buffer && depth_stencil_buffer)
  {
    if (color_buffer->GetSwapChain () != depth_stencil_buffer->GetSwapChain ())
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported render targets configuration. Render-target-view and depth-stencil view have different swap chains");

    return;
  }
  
    //проверка взаимодействия со вспомогательным буфером попиксельного отсечения

  if (fake_depth_stencil_buffer && color_buffer)
  {
    if (!color_buffer->IsShadow ())
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported render targets configuration. Render-target-view is not pbuffer");

    return;
  }

  if (fake_depth_stencil_buffer && !color_buffer)
  {
    manager.GetShadowBuffers (color_buffer, depth_stencil_buffer);

    return;
  }

    //проверка случаев отсутствия одного из буферов  

  if (color_buffer && !depth_stencil_buffer)
  {
      //отсутствует буфер попиксельного отсечения, но присутствует буфер цвета

    depth_stencil_buffer = manager.GetShadowBuffer (color_buffer.get ());

    return;
  }
  
  if (!color_buffer && depth_stencil_buffer)
  {
      //отсутствует буфер цвета, но присутствует буфер попиксельного отсечения

    color_buffer = manager.GetShadowBuffer (depth_stencil_buffer.get ());    
    
    return;
  }

    //отсутствуют оба буфера (рендеринг в обе целевые текстуры)    

  manager.GetShadowBuffers (color_buffer, depth_stencil_buffer);    
}

/*
    Установка буфера в контекст OpenGL
*/

void SwapChainFrameBuffer::Bind ()
{
  try
  {
    frame_buffer_manager.SetFrameBuffer (color_buffer->GetSwapChain (), color_buffer->GetBufferType ());
    frame_buffer_manager.SetFrameBufferActivity (is_buffer_active [RenderTargetType_Color],
                                                 is_buffer_active [RenderTargetType_DepthStencil]);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainFrameBuffer::Bind");
    throw;
  }
}

/*
    Оповещение об отрисовке в целевые буферы
*/

void SwapChainFrameBuffer::InvalidateRenderTargets (const Rect& update_rect)
{
  if (update_rect.x < dirty_rect.x)
  {
    dirty_rect.width += dirty_rect.x - update_rect.x;
    dirty_rect.x      = update_rect.x;
  }

  if (update_rect.y < dirty_rect.y)
  {
    dirty_rect.height += dirty_rect.y - update_rect.y;
    dirty_rect.y       = update_rect.y;
  }

  size_t right  = update_rect.x + update_rect.width,
         bottom = update_rect.y + update_rect.height;

  if (right > dirty_rect.x + dirty_rect.width)
    dirty_rect.width = right - dirty_rect.x;
    
  if (bottom > dirty_rect.y + dirty_rect.height)
    dirty_rect.height = bottom - dirty_rect.y;
}

void SwapChainFrameBuffer::InvalidateRenderTargets ()
{
  size_t color_width  = render_targets [RenderTargetType_Color].mip_level_desc.width,
         color_height = render_targets [RenderTargetType_Color].mip_level_desc.height,
         ds_width     = render_targets [RenderTargetType_DepthStencil].mip_level_desc.width,
         ds_height    = render_targets [RenderTargetType_DepthStencil].mip_level_desc.height;

  dirty_rect.x      = 0;
  dirty_rect.y      = 0;
  dirty_rect.width  = color_width < ds_width ? ds_width : color_width;
  dirty_rect.height = color_height < ds_height ? ds_height : color_height;
}

/*
    Обновление целевых текстур
*/

void SwapChainFrameBuffer::UpdateRenderTargets ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::UpdateRenderTargets";

    //если целевые текстуры отсутствуют или область обновления пуста, то обновление не нужно

  if (!has_texture_targets || !dirty_rect.width || !dirty_rect.height)
    return;

    //установка активного буфера кадра

  Bind ();
  
    //копирование изображений из различных целевых буферов в текстуру
        
  for (size_t i=0; i<RenderTargetType_Num; i++)
  {
    RenderTarget& render_target = render_targets [i];

    if (!render_target.target_texture)
      continue;

      //получение параметров целевой текстуры

    const ViewDesc&                view_desc      = render_target.view_desc;
    const MipLevelDesc&            mip_level_desc = render_target.mip_level_desc;
    const RenderTargetTextureDesc& texture_desc   = render_target.texture_desc;

      //отсечение
      
    size_t x      = dirty_rect.x,
           y      = dirty_rect.y,
           width  = dirty_rect.width,
           height = dirty_rect.height;

    if (x > mip_level_desc.width || y > mip_level_desc.height)
      continue;

    if (x + width > mip_level_desc.width)
      width = mip_level_desc.width - x;

    if (y + height > mip_level_desc.height)
      height = mip_level_desc.height - y;

    if (!width || !height)
      continue;

      //установка текущей текстуры

    render_target.target_texture->Bind ();

      //копирование изображения в текстуру
      
    GLenum tex_target = texture_desc.target;

    switch (tex_target)
    {
      case GL_TEXTURE_1D:
        glCopyTexSubImage1D (GL_TEXTURE_1D, view_desc.mip_level, x, x, y, width);
        break;
      case GL_TEXTURE_CUBE_MAP:
        tex_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + view_desc.layer;
      case GL_TEXTURE_2D:
      case GL_TEXTURE_RECTANGLE_ARB:
        glCopyTexSubImage2D (tex_target, view_desc.mip_level, x, y, x, y, width, height);
        break;
      case GL_TEXTURE_3D:
        glCopyTexSubImage3D (tex_target, view_desc.mip_level, x, y, view_desc.layer, x, y, width, height);
        break;
      default:
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported texture target 0x%04x", tex_target);
        break;
    }
  }
  
    //проверка ошибок
  
  CheckErrors (METHOD_NAME);

    //очистка области обновления

  memset (&dirty_rect, 0, sizeof dirty_rect);
}
