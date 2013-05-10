#include "shared.h"
#include <common/hash.h>

using namespace common;

const size_t TEX_SIZE        = 128;
const size_t IMAGE_DATA_SIZE = TEX_SIZE * TEX_SIZE * 4;

void log_print (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
}

int main ()
{
  printf ("Results of texture_sampler_test:\n");

  common::LogFilter filter ("*", &log_print);
  
  try
  {
    Test test (L"DX11 device test window (texture_sampler_test)", "debug=1");

    SamplerDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.min_filter           = TexMinFilter_LinearMipLinear;
    desc.max_anisotropy       = 1;
    desc.mag_filter           = TexMagFilter_Linear;
    desc.wrap_u               = TexcoordWrap_Clamp;
    desc.wrap_v               = TexcoordWrap_Clamp;
    desc.comparision_function = CompareMode_AlwaysPass;
    desc.min_lod              = 0;
    desc.max_lod              = FLT_MAX;

    xtl::com_ptr<ISamplerState> sampler (test.device->CreateSamplerState (desc), false);

    TextureDesc tex_desc;
    memset (&tex_desc, 0, sizeof (tex_desc));
  
    char* image_data = new char [IMAGE_DATA_SIZE];
    
    tex_desc.dimension            = TextureDimension_2D;
    tex_desc.width                = TEX_SIZE;
    tex_desc.height               = TEX_SIZE;
    tex_desc.layers               = 1;
    tex_desc.format               = PixelFormat_RGBA8;
    tex_desc.bind_flags           = BindFlag_Texture;
    tex_desc.access_flags         = AccessFlag_Write;
    tex_desc.generate_mips_enable = false;
    tex_desc.usage_mode           = UsageMode_Dynamic;

    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (tex_desc), false);

    memset (image_data, 17, IMAGE_DATA_SIZE);

    texture->SetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_RGB8, image_data);

    test.device->GetImmediateContext ()->SSSetSampler (1, sampler.get ());
    test.device->GetImmediateContext ()->SSSetTexture  (1, texture.get ());

    test.device->GetImmediateContext ()->Draw (PrimitiveType_PointList, 0, 0);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
