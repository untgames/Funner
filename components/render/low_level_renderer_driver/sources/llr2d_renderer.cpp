#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;

namespace
{

render::low_level::PixelFormat get_pixel_format (media::PixelFormat format)
{
  switch (format)
  {
    case media::PixelFormat_RGB8:  return render::low_level::PixelFormat_RGB8;
    case media::PixelFormat_RGBA8: return render::low_level::PixelFormat_RGBA8;
    default:
      throw xtl::format_operation_exception ("get_pixel_format", "Can't convert media format %s to render format.", media::get_format_name (format));
  }
}

void default_error_log (const char* message)
{
  common::LogSystem::Print ("render.mid_level.low_level_driver.renderer2d.Renderer", message);
}

const char* SHADER_SOURCE_CODE =
"Parameters\n"
"{\n"
"  float4x4 currentViewMatrix currentObjectMatrix currentProjMatrix\n"
"}\n"
"ProjectionMatrix currentProjMatrix\n"
"ViewMatrix       currentViewMatrix\n"
"AlphaCompareMode AlwaysPass\n"
"ObjectMatrix     currentObjectMatrix\n";

}

/*
    Конструктор
*/

Renderer::Renderer (render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain)
  : BasicRenderer (device, swap_chain)
{
  try
  {
    ShaderDesc shader_desc = {"render.mid_level.low_level_driver.renderer2d.Renderer.common_shader", strlen (SHADER_SOURCE_CODE), SHADER_SOURCE_CODE, "fpp", ""};

    program = device->CreateProgram (1, &shader_desc, &default_error_log);

    device->SSSetProgram (program.get ());

    ProgramParameter program_parameters[3] = { {"currentViewMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (ProgramParameters, view_matrix)}, 
                                               {"currentObjectMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (ProgramParameters, object_matrix)},
                                               {"currentProjMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (ProgramParameters, projection_matrix)} };

    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof (program_parameters) / sizeof (program_parameters[0]), program_parameters};

    program_parameters_layout = device->CreateProgramParametersLayout (program_parameters_layout_desc);

    device->SSSetProgramParametersLayout (program_parameters_layout.get ());

    SamplerDesc sampler_desc;
    
    memset (&sampler_desc, 0, sizeof (sampler_desc));

    sampler_desc.min_filter = TexMinFilter_LinearMipLinear;
    sampler_desc.mag_filter = TexMagFilter_Linear;
    sampler_desc.max_anisotropy = 1;
    sampler_desc.wrap_u     = TexcoordWrap_Repeat;
    sampler_desc.wrap_v     = TexcoordWrap_Repeat;
    sampler_desc.comparision_function = CompareMode_AlwaysPass;
    sampler_desc.min_lod    = 0;
    sampler_desc.max_lod    = FLT_MAX;

    sampler = device->CreateSamplerState (sampler_desc);

    device->SSSetSampler (0, sampler.get ());
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
  TextureDesc texture_desc = {TextureDimension_2D, image.Width (), image.Height (), 1, get_pixel_format (image.Format ()), true, AccessFlag_ReadWrite, BindFlag_Texture, UsageMode_Default};

  xtl::com_ptr<render::low_level::ITexture> new_texture (device->CreateTexture (texture_desc), false);

  new_texture->SetData (0, 0, 0, 0, image.Width (), image.Height (), get_pixel_format (image.Format ()), image.Bitmap ());

  return new ImageTexture (new_texture.get ());
}

render::mid_level::renderer2d::ITexture* Renderer::CreateTexture (size_t width, size_t height, media::PixelFormat pixel_format)
{
  TextureDesc texture_desc = {TextureDimension_2D, width, height, 1, get_pixel_format (pixel_format), false, AccessFlag_ReadWrite, BindFlag_Texture | BindFlag_RenderTarget, UsageMode_Default};
  ViewDesc    view_desc = {0, 0};

  return new RenderTargetTexture (device->CreateView (device->CreateTexture (texture_desc), view_desc));
}

render::mid_level::renderer2d::IPrimitive* Renderer::CreatePrimitive ()
{
  return new Primitive;
}

render::mid_level::renderer2d::IFrame* Renderer::CreateFrame ()
{
  return new Frame;
}
