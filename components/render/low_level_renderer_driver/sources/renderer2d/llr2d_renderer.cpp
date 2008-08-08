#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;

/*
    Конструктор
*/

Renderer::Renderer (IDevice* device, size_t swap_chains_count, ISwapChain** swap_chains)
  : BasicRenderer (device, swap_chains_count, swap_chains)
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

/*
    Создание системы рендеринга
*/

namespace render
{

namespace mid_level
{

namespace low_level_driver
{

IRenderer* create_renderer2d (IDevice* device, size_t swap_chains_count, ISwapChain** swap_chains)
{
  return new Renderer (device, swap_chains_count, swap_chains);
}

}

}

}
