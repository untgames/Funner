#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;

/*
    Конструктор
*/

Renderer::Renderer (render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain)
  : BasicRenderer (device, swap_chain)
{
  try
  {
    common_resources = CommonResourcesPtr (new CommonResources (device), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level:low_level_driver::renderer2d::Renderer::Renderer");
    throw;
  }
}

/*
    Создание ресурсов
*/

render::mid_level::renderer2d::ITexture* Renderer::CreateTexture (const media::Image& image)
{
  try
  {
    return new ImageTexture (*device, image);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::low_level_driver::renderer2d::Renderer::CreateTexture(const media::Image&)");
    throw;
  }
}

render::mid_level::renderer2d::ITexture* Renderer::CreateTexture (size_t width, size_t height, media::PixelFormat pixel_format)
{
  try
  {
    return new RenderTargetTexture (*device, width, height, pixel_format);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::low_level_driver::renderer2d::Renderer::CreateTexture(size_t, size_t, media::PixelFormat)");
    throw;
  }
}

render::mid_level::renderer2d::IPrimitive* Renderer::CreatePrimitive ()
{
  return new Primitive;
}

render::mid_level::renderer2d::IFrame* Renderer::CreateFrame ()
{
  return new Frame (common_resources.get (), device.get ());
}
