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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Общие ресурсы
///////////////////////////////////////////////////////////////////////////////////////////////////
class CommonResources : public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    CommonResources (render::low_level::IDevice* device);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение низкоуровневых данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::IBlendState*        GetBlendState (render::mid_level::renderer2d::BlendMode blend_mode) { return blend_states[blend_mode].get (); }
    render::low_level::IBuffer*            GetConstantBuffer () { return constant_buffer.get (); }
    render::low_level::IDepthStencilState* GetDepthStencilState (bool depth_write_enabled);

  private:
    typedef xtl::com_ptr<render::low_level::IBlendState>        BlendStatePtr;
    typedef xtl::com_ptr<render::low_level::IBuffer>            BufferPtr;
    typedef xtl::com_ptr<render::low_level::IDepthStencilState> DepthStencilStatePtr;
  
  private:
    BlendStatePtr        blend_states[render::mid_level::renderer2d::BlendMode_Num];
    BufferPtr            constant_buffer;
    DepthStencilStatePtr depth_stencil_states[2];
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система рендеринга 2D-примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
class Renderer: virtual public mid_level::renderer2d::IRenderer, public BasicRenderer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Renderer  (render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    mid_level::renderer2d::ITexture*   CreateTexture   (const media::Image& image);
    mid_level::renderer2d::ITexture*   CreateTexture   (size_t width, size_t height, media::PixelFormat pixel_format);
    mid_level::renderer2d::IPrimitive* CreatePrimitive ();
    mid_level::renderer2d::IFrame*     CreateFrame     ();

  private:
    typedef xtl::com_ptr<render::low_level::IProgram>                 ProgramPtr;
    typedef xtl::com_ptr<render::low_level::IProgramParametersLayout> ProgramParametersLayoutPtr;
    typedef xtl::com_ptr<render::low_level::ISamplerState>            SamplerStatePtr;
    typedef xtl::com_ptr<render::low_level::IInputLayout>             InputLayoutPtr;
    typedef xtl::com_ptr<CommonResources>                             CommonResourcesPtr;

  private:
    ProgramPtr                 program;
    ProgramParametersLayoutPtr program_parameters_layout;
    SamplerStatePtr            sampler;
    InputLayoutPtr             input_layout;
    CommonResourcesPtr         common_resources;
};


}

}

}

}

#endif
