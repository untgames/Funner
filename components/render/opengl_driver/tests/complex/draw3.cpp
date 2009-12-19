#include "shared.h"

const char* PIXEL_SHADER_FILE_NAME  = "data/glsl/bumpslc.frag";
const char* VERTEX_SHADER_FILE_NAME = "data/glsl/bumpslc.vert";
const char* BUMP_IMAGE_NAME         = "data/bump.tga";
const char* DIFFUSE_IMAGE_NAME      = "data/diffuse.tga";

#pragma pack (1)

struct Vec3f
{
  float x, y, z;
};

struct Vec2f
{
  float s, t;
};

struct MyVertex
{
  Vec3f position;
  Vec3f normal;
  Vec2f tc;
  Vec3f color;
};

struct MyShaderParameters
{
  math::vec3f light_position;
  int         bump_texture_slot;
  int         diffuse_texture_slot;
};

struct MyShaderParameters2
{
  math::mat4f transform;
};

void redraw (Test& test)
{
  test.device->Draw (PrimitiveType_TriangleStrip, 0, 4);
}

void idle (Test& test)
{
  if (test.window.IsClosed ())
    return;

  static size_t last = 0;
  static float angle;

  if (common::milliseconds () - last > 25)
  {
    last = common::milliseconds ();
    return;
  }

  MyShaderParameters2 my_shader_parameters2;

  IBuffer* cb = test.device->SSGetConstantBuffer (1);

  if (!cb)
  {
    printf ("Null constant buffer #1\n");
    return;
  }

  cb->GetData (0, sizeof my_shader_parameters2, &my_shader_parameters2);

//  my_shader_parameters2.transform = math::rotatef (math::deg2rad (angle+=1.f), 0, 0, 1);
  my_shader_parameters2.transform = math::rotate (math::degree (angle+=0.5f), math::vec3f (1, 1, 0));

  cb->SetData (0, sizeof my_shader_parameters2, &my_shader_parameters2);

  test.window.Invalidate ();
}

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

int main ()
{
  printf ("Results of draw3_test:\n");

  try
  {
    Test test (L"OpenGL device test window (draw3)", &redraw);

    test.window.Show ();

    printf ("Create vertex buffer\n");

    static const MyVertex verts [] = {
      {{ 1, -1, 0}, {0, 0, 1}, {1, 1}, {1, 0, 0}},
      {{-1, -1, 0}, {0, 0, 1}, {0, 1}, {1, 0, 0}},
      {{ 1,  1, 0}, {0, 0, 1}, {1, 0}, {1, 0, 0}},
      {{-1,  1, 0}, {0, 0, 1}, {0, 0}, {1, 0, 0}},
    };

    static const size_t VERTICES_COUNT = sizeof (verts) / sizeof (MyVertex);

    BufferDesc vb_desc;

    memset (&vb_desc, 0, sizeof vb_desc);

    vb_desc.size         = sizeof (MyVertex) * VERTICES_COUNT;
    vb_desc.usage_mode   = UsageMode_Default;
    vb_desc.bind_flags   = BindFlag_VertexBuffer;
    vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

    BufferPtr vb (test.device->CreateBuffer (vb_desc), false);

    vb->SetData (0, vb_desc.size, verts);

    printf ("Set input-stage\n");

    VertexAttribute attributes [] = {
      {VertexAttributeSemantic_Normal, InputDataFormat_Vector3, InputDataType_Float, 0, TEST_OFFSETOF (MyVertex, normal), sizeof (MyVertex)},
      {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, TEST_OFFSETOF (MyVertex, position), sizeof (MyVertex)},
      {VertexAttributeSemantic_Color, InputDataFormat_Vector3, InputDataType_Float, 0, TEST_OFFSETOF (MyVertex, color), sizeof (MyVertex)},
//      {VertexAttributeSemantic_TexCoord0, InputDataFormat_Vector2, InputDataType_Float, 0, TEST_OFFSETOF (MyVertex, tc), sizeof (MyVertex)},
    };

    InputLayoutDesc layout_desc;

    memset (&layout_desc, 0, sizeof layout_desc);

    layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
    layout_desc.vertex_attributes       = attributes;
    layout_desc.index_type              = InputDataType_UInt;
    layout_desc.index_buffer_offset     = 0;

    InputLayoutPtr layout (test.device->CreateInputLayout (layout_desc), false);

    test.device->ISSetInputLayout (layout.get ());
    test.device->ISSetVertexBuffer (0, vb.get ());

    printf ("Set shader stage\n");

    stl::string pixel_shader_source  = read_shader (PIXEL_SHADER_FILE_NAME),
                vertex_shader_source = read_shader (VERTEX_SHADER_FILE_NAME);

    ShaderDesc shader_descs [] = {
      {"p_shader", size_t (-1), pixel_shader_source.c_str (), "glsl.ps", ""},
      {"v_shader", size_t (-1), vertex_shader_source.c_str (), "glsl.vs", ""}
    };

    static ProgramParameter shader_parameters[] = {
      {"LightPosition", ProgramParameterType_Float3, 0, 1, TEST_OFFSETOF (MyShaderParameters, light_position)},
      {"bump_sampler2d", ProgramParameterType_Int, 0, 1, TEST_OFFSETOF (MyShaderParameters, bump_texture_slot)},
      {"diffuse_sampler2d", ProgramParameterType_Int, 0, 1, TEST_OFFSETOF (MyShaderParameters, diffuse_texture_slot)},
      {"Transform", ProgramParameterType_Float4x4, 1, 1, TEST_OFFSETOF (MyShaderParameters2, transform)}
    };

    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

    ProgramPtr shader (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print), false);
    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

    BufferDesc cb_desc;

    memset (&cb_desc, 0, sizeof cb_desc);

    cb_desc.size         = sizeof (MyShaderParameters);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    BufferDesc cb_desc2 (cb_desc);

    cb_desc2.size = sizeof (MyShaderParameters2);

    BufferPtr cb (test.device->CreateBuffer (cb_desc), false), cb2 (test.device->CreateBuffer (cb_desc2), false);

    MyShaderParameters my_shader_parameters = {
      math::vec3f (0, 0, 10),
      0,
      1
    };

    MyShaderParameters2 my_shader_parameters2 = {1.0f};

    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);
    cb2->SetData (0, sizeof my_shader_parameters2, &my_shader_parameters2);

    test.device->SSSetProgram (shader.get ());
    test.device->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test.device->SSSetConstantBuffer (0, cb.get ());
    test.device->SSSetConstantBuffer (1, cb2.get ());    

    printf ("Load textures\n");

    media::Image bump_image (BUMP_IMAGE_NAME, media::PixelFormat_RGB8);
    media::Image diffuse_image (DIFFUSE_IMAGE_NAME, media::PixelFormat_RGB8);

    printf ("Textures loaded\n");

    TextureDesc texture_desc;

    memset (&texture_desc, 0, sizeof texture_desc);

    texture_desc.dimension            = TextureDimension_2D;
    texture_desc.width                = bump_image.Width ();
    texture_desc.height               = bump_image.Height ();
    texture_desc.layers               = 1;
    texture_desc.format               = PixelFormat_RGB8;
    texture_desc.bind_flags           = BindFlag_Texture;
    texture_desc.access_flags         = AccessFlag_ReadWrite;
    texture_desc.generate_mips_enable = true;

    SamplerDesc sampler_desc;

    memset (&sampler_desc, 0, sizeof sampler_desc);

    sampler_desc.min_filter     = TexMinFilter_LinearMipLinear;
    sampler_desc.max_anisotropy = 16;
    sampler_desc.mag_filter     = TexMagFilter_Linear;
    sampler_desc.wrap_u         = TexcoordWrap_Clamp;
    sampler_desc.wrap_v         = TexcoordWrap_Clamp;
    sampler_desc.wrap_w         = TexcoordWrap_Clamp;

    TexturePtr bump_texture (test.device->CreateTexture (texture_desc), false);

    texture_desc.width  = diffuse_image.Width ();
    texture_desc.height = diffuse_image.Height ();

    TexturePtr diffuse_texture (test.device->CreateTexture (texture_desc), false);
    SamplerStatePtr sampler (test.device->CreateSamplerState (sampler_desc), false);

    bump_texture->SetData (0, 0, 0, 0, bump_image.Width (), bump_image.Height (), PixelFormat_RGB8, bump_image.Bitmap ());
    diffuse_texture->SetData (0, 0, 0, 0, diffuse_image.Width (), diffuse_image.Height (), PixelFormat_RGB8, diffuse_image.Bitmap ());

    test.device->SSSetTexture (0, bump_texture.get ());
    test.device->SSSetSampler (0, sampler.get ());
    test.device->SSSetTexture (1, diffuse_texture.get ());
    test.device->SSSetSampler (1, sampler.get ());

    printf ("Register callbacks\n");

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));

    printf ("Main loop\n");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
