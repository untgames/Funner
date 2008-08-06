#ifndef RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_2D_RENDERER_HEADER
#define RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_2D_RENDERER_HEADER

#include <render/mid_level/renderer2d.h>

#include <shared/basic_renderer.h>

namespace render
{

namespace mid_level
{

namespace low_level_driver
{

namespace renderer2d
{

//forward declarations
class CommonResources;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система рендеринга 2D-примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
class Renderer: virtual public mid_level::renderer2d::IRenderer, public BasicRenderer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Renderer (render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    mid_level::renderer2d::ITexture*   CreateTexture   (const media::Image& image);
    mid_level::renderer2d::ITexture*   CreateTexture   (size_t width, size_t height, media::PixelFormat pixel_format);
    mid_level::renderer2d::IPrimitive* CreatePrimitive ();
    mid_level::renderer2d::IFrame*     CreateFrame     ();

  private:
    typedef xtl::com_ptr<CommonResources> CommonResourcesPtr;

  private:
    CommonResourcesPtr common_resources;
};

}

}

}

}

#endif
