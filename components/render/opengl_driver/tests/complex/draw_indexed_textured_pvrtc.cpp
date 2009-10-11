#include "shared.h"

const char* IMAGE_NAME = "data/pic1.pvr";

struct Vec3f
{
  float x, y, z;
};

struct Vec2f
{
  float x, y;
};

struct Color4ub
{
  unsigned char red, green, blue, alpha;
};

struct MyVertex
{
  Vec3f    position;
  Vec3f    normal;
  Vec2f    tcoord;
  Color4ub color;
};

void redraw (Test& test)
{
  test.device->DrawIndexed (PrimitiveType_TriangleList, 0, 6, 0);  
}

PixelFormat get_format (const char* name)
{
  if      (!strcmp (name, "rgb_pvrtc2"))  return PixelFormat_RGB_PVRTC2;
  else if (!strcmp (name, "rgb_pvrtc4"))  return PixelFormat_RGB_PVRTC4;
  else if (!strcmp (name, "rgba_pvrtc2")) return PixelFormat_RGBA_PVRTC2;
  else if (!strcmp (name, "rgba_pvrtc4")) return PixelFormat_RGBA_PVRTC4;

  throw xtl::format_not_supported_exception ("::get_format", "Unsupported compression format '%s'", name);
}

int main ()
{
  printf ("Results of draw_indexed_textured_pvrtc_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (draw_indexed_textured)", &redraw);

    test.window.Show ();
   
    printf ("Create vertex buffer\n");
    
    static const MyVertex verts [] = {
      {{-1, -1, 0}, {0, 0, 1}, {0, 0}, {255, 255, 255, 255}},
      {{ 1, -1, 0}, {0, 0, 1}, {1, 0}, {255, 255, 255, 255}},
      {{ 1, 1, 0},  {0, 0, 1}, {1, 1}, {255, 255, 255, 255}},
      {{-1, 1, 0},  {0, 0, 1}, {0, 1}, {255, 255, 255, 255}},
    };    
    
    static const size_t VERTICES_COUNT = sizeof verts / sizeof *verts;
    
    BufferDesc vb_desc;
    
    memset (&vb_desc, 0, sizeof vb_desc);
    
    vb_desc.size         = sizeof (MyVertex) * VERTICES_COUNT;
    vb_desc.usage_mode   = UsageMode_Default;
    vb_desc.bind_flags   = BindFlag_VertexBuffer;
    vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;
    
    BufferPtr vb (test.device->CreateBuffer (vb_desc), false);    
    
    vb->SetData (0, vb_desc.size, verts);
    
    printf ("Create index buffer\n");    

    static unsigned short indices [] = {0, 1, 2, 3, 0, 2};

    BufferDesc ib_desc;

    memset (&ib_desc, 0, sizeof ib_desc);

    ib_desc.size         = sizeof indices;
    ib_desc.usage_mode   = UsageMode_Default;
    ib_desc.bind_flags   = BindFlag_IndexBuffer;
    ib_desc.access_flags = AccessFlag_ReadWrite;

    BufferPtr ib (test.device->CreateBuffer (ib_desc), false);

    ib->SetData (0, ib_desc.size, indices);  

    printf ("Set input-stage\n");
    
    VertexAttribute attributes [] = {
      {VertexAttributeSemantic_Normal, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, normal), sizeof (MyVertex)},
      {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, position), sizeof (MyVertex)},
      {VertexAttributeSemantic_Color, InputDataFormat_Vector4, InputDataType_UByte, 0, offsetof (MyVertex, color), sizeof (MyVertex)},
      {VertexAttributeSemantic_TexCoord0, InputDataFormat_Vector2, InputDataType_Float, 0, offsetof (MyVertex, tcoord), sizeof (MyVertex)},
    };
    
    InputLayoutDesc layout_desc;
    
    memset (&layout_desc, 0, sizeof layout_desc);
    
    layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
    layout_desc.vertex_attributes       = attributes;
    layout_desc.index_type              = InputDataType_UShort;
    layout_desc.index_buffer_offset     = 0;
    
    InputLayoutPtr layout (test.device->CreateInputLayout (layout_desc), false);

    test.device->ISSetInputLayout (layout.get ());
    test.device->ISSetVertexBuffer (0, vb.get ());
    test.device->ISSetIndexBuffer (ib.get ());
    
    printf ("Load textures\n");
    
    media::CompressedImage image (IMAGE_NAME);

    TextureDesc texture_desc;
    
    memset (&texture_desc, 0, sizeof texture_desc);
    
    texture_desc.dimension            = TextureDimension_2D;
    texture_desc.width                = image.Width ();
    texture_desc.height               = image.Height ();
    texture_desc.layers               = 1;
    texture_desc.format               = get_format (image.Format ());
    texture_desc.bind_flags           = BindFlag_Texture;
    texture_desc.access_flags         = AccessFlag_ReadWrite;
    texture_desc.generate_mips_enable = false;
    
    SamplerDesc sampler_desc;
    
    memset (&sampler_desc, 0, sizeof sampler_desc);
    
    sampler_desc.min_filter     = TexMinFilter_Linear;
    sampler_desc.max_anisotropy = 1;
    sampler_desc.mag_filter     = TexMagFilter_Linear;
    sampler_desc.wrap_u         = TexcoordWrap_Repeat;
    sampler_desc.wrap_v         = TexcoordWrap_Repeat;
    sampler_desc.wrap_w         = TexcoordWrap_Repeat;
    sampler_desc.comparision_function = CompareMode_AlwaysPass;
    sampler_desc.min_lod        = 0;
    sampler_desc.max_lod        = FLT_MAX;    
    
    TextureData texture_data;
    
    memset (&texture_data, 0, sizeof (TextureData));
    
    texture_data.data = image.Data ();

    TexturePtr      texture (test.device->CreateTexture (texture_desc, texture_data), false);
    SamplerStatePtr sampler (test.device->CreateSamplerState (sampler_desc), false);

    test.device->SSSetTexture (0, texture.get ());
    test.device->SSSetSampler (0, sampler.get ());

    printf ("Main loop\n");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
