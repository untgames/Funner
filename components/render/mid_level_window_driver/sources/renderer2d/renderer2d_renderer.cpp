#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::window_driver;
using namespace render::mid_level::window_driver::renderer2d;

namespace
{

typedef xtl::com_ptr<render::low_level::IDevice> DevicePtr;
typedef xtl::com_ptr<CommonResources>  CommonResourcesPtr;

}

struct Renderer2D::Impl
{
  CommonResourcesPtr common_resources;
  DevicePtr          device;

  Impl (IDevice* in_device) : common_resources (CommonResourcesPtr (new CommonResources (in_device), false)), device (in_device) {}
};

/*
    Конструктор
*/

Renderer2D::Renderer2D (IDevice* device)
  : impl (new Impl (device))
  {}

Renderer2D::~Renderer2D ()
{
}

/*
    Создание ресурсов
*/

render::mid_level::renderer2d::ITexture* Renderer2D::CreateTexture (const media::Image& image)
{
  try
  {
    return new ImageTexture (*impl->device, image);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::window_driver::renderer2d::Renderer2D::CreateTexture(const media::Image&)");
    throw;
  }
}

render::mid_level::renderer2d::ITexture* Renderer2D::CreateTexture (const media::CompressedImage& image)
{
  try
  {
    return new ImageTexture (*impl->device, image);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::window_driver::renderer2d::Renderer2D::CreateTexture(const media::CompressedImage&)");
    throw;
  }
}


render::mid_level::renderer2d::ITexture* Renderer2D::CreateTexture (size_t width, size_t height, render::mid_level::PixelFormat pixel_format)
{
  try
  {
    return new RenderTargetTexture (*impl->device, width, height, pixel_format);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::window_driver::renderer2d::Renderer2D::CreateTexture(size_t, size_t, media::PixelFormat)");
    throw;
  }
}

render::mid_level::renderer2d::IPrimitive* Renderer2D::CreatePrimitive ()
{
  return new Primitive;
}

render::mid_level::renderer2d::IFrame* Renderer2D::CreateFrame ()
{
  return new Frame (impl->common_resources.get (), impl->device.get ());
}
