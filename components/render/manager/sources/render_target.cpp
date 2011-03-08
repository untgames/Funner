#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    ќписание реализации цели рендеринга
*/

struct RenderTargetImpl::Impl
{
  DeviceManagerPtr    device_manager; //менеджер устройства отрисовки
  LowLevelTexturePtr  texture;        //целева€ текстура
  LowLevelViewPtr     view;           //отображение дл€ рендеринга в текстуру
  size_t              width;          //ширина цели рендеринга
  size_t              height;         //высота цели рендеринга
  
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , width (0)
    , height (0)
  {
  }
};

/*
     онструктор / деструктор
*/

RenderTargetImpl::RenderTargetImpl (const DeviceManagerPtr& device_manager, render::low_level::ITexture* texture, size_t layer, size_t mip_level)
{
  try
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
      
    if (!texture)
      throw xtl::make_null_argument_exception ("", "texture");
    
    impl = new Impl (device_manager);
    
    SetTarget (texture, layer, mip_level);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderTargetImpl::RenderTargetImpl");
    throw;
  }
}

RenderTargetImpl::~RenderTargetImpl ()
{
}

/*
    ѕолучение отображени€ (в случае отсутстви€ - исключение)
*/

render::low_level::IView& RenderTargetImpl::View ()
{
  if (!impl->view)
    throw xtl::format_operation_exception ("render::mip_level::RenderTargetImpl::View", "No view binded");
    
  return *impl->view;
}

/*
    ќбновление текстуры
*/

void RenderTargetImpl::SetTarget (render::low_level::ITexture* texture, size_t layer, size_t mip_level)
{
  try
  {
    if (!texture)
    {
      impl->texture = 0;
      impl->view    = 0;
      impl->width   = 0;
      impl->height  = 0;

      return;
    }
    
    low_level::ViewDesc view_desc;
    
    memset (&view_desc, 0, sizeof view_desc);
    
    view_desc.layer     = layer;
    view_desc.mip_level = mip_level;
    
    LowLevelViewPtr view (impl->device_manager->Device ().CreateView (texture, view_desc), false);
    
    low_level::TextureDesc texture_desc;
    
    texture->GetDesc (texture_desc);
    
    impl->width   = texture_desc.width;
    impl->height  = texture_desc.height;
    impl->view    = view;
    impl->texture = texture;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mip_level::RenderTargetImpl::SetTarget");
    throw;
  }
}

/*
    –азмеры цели рендеринга
*/

void RenderTargetImpl::Resize (size_t width, size_t height)
{
  impl->width  = width;
  impl->height = height;
}

size_t RenderTargetImpl::Width ()
{
  return impl->width;
}

size_t RenderTargetImpl::Height ()
{
  return impl->height;
}

/*
    «ахват изображени€
*/

void RenderTargetImpl::Capture (media::Image& image)
{
  try
  {
    if (!impl->texture)
      throw xtl::format_operation_exception ("", "No texture binded");
    
    render::low_level::TextureDesc texture_desc;

    impl->texture->GetDesc (texture_desc);    
    
    media::PixelFormat image_format;

    switch (texture_desc.format)
    {
      case render::low_level::PixelFormat_RGB8:
        image_format = media::PixelFormat_RGB8;
        break;
      case render::low_level::PixelFormat_RGBA8:
        image_format = media::PixelFormat_RGBA8;
        break;
      default:
        throw xtl::format_not_supported_exception ("", "Texture uses incompatible format %s", render::low_level::get_name (texture_desc.format));
    }    

    media::Image result_image (texture_desc.width, texture_desc.height, 1, image_format);

    impl->texture->GetData (0, 0, 0, 0, texture_desc.width, texture_desc.height, texture_desc.format, result_image.Bitmap ());    
    
    result_image.Swap (image);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderTargetImpl::Capture");
    throw;
  }
}
