#include "shared.h"

const char* PIXEL_SHADER_FILE_NAME  = "data/glsl/wood.frag";
const char* VERTEX_SHADER_FILE_NAME = "data/glsl/wood.vert";

struct Vec3f
{
  float x, y, z;
};

struct Color4ub
{
  unsigned char red, green, blue, alpha;
};

struct MyVertex
{
  Vec3f    position;
  Vec3f    normal;
  Color4ub color;
};

#pragma pack (1)

struct MyShaderParameters
{
  float        grain_size_recip;
  math::vec3f  dark_color;
  math::vec3f  color_spread;
  math::vec3f  light_position;
  float        scale;
};

struct MyShaderParameters2
{
  math::mat4f  transform;
};

void redraw (Test& test)
{
  test.device->Draw (PrimitiveType_TriangleList, 0, 3);
}

void idle (Test& test)
{
  if (test.window.IsClosed ())
    return;

  static const   float DT = 0.01f;
  static float   t = 0;
  static clock_t last = 0;
  static float angle;

/*  if (clock () - last < CLK_TCK / 30)
  {
    last = clock ();
    return;
  }*/

  MyShaderParameters2 my_shader_parameters2;
  
  IBuffer* cb = test.device->SSGetConstantBuffer (1);
  
  if (!cb)
  {
    printf ("Null constant buffer #1\n");
    return;
  }
  
  cb->GetData (0, sizeof my_shader_parameters2, &my_shader_parameters2);
    
  my_shader_parameters2.transform = math::rotatef (math::deg2rad (angle+=1.f), 0, 0, 1);
//  my_shader_parameters.transform *= math::rotatef (math::deg2rad (.3f), 0, 0, 1);

  cb->SetData (0, sizeof my_shader_parameters2, &my_shader_parameters2);
 
  test.window.Invalidate ();
}

ViewPtr create_depth_stencil_view (IDevice* device, ISwapChain* swap_chain)
{
  TexturePtr depth_stencil_texture (device->CreateDepthStencilTexture (swap_chain), false);

  ViewDesc depth_stencil_view_desc;

  memset (&depth_stencil_view_desc, 0, sizeof (depth_stencil_view_desc));

  return ViewPtr (device->CreateView (depth_stencil_texture.get (), depth_stencil_view_desc), false);
}

ViewPtr create_render_view (IDevice* device, ISwapChain* swap_chain)
{
  TexturePtr render_target_texture (device->CreateRenderTargetTexture (swap_chain, 1), false);

  ViewDesc render_target_view_desc;

  memset (&render_target_view_desc, 0, sizeof (render_target_view_desc));

  return ViewPtr (device->CreateView (render_target_texture.get (), render_target_view_desc), false);
}

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

int main ()
{
  printf ("Results of draw2_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (draw2)", &redraw, "check_gl_errors=0");
    
    test.window.Show ();
   
    printf ("Create vertex buffer\n");
    
    static const size_t VERTICES_COUNT = 3;
    
    BufferDesc vb_desc;
    
    memset (&vb_desc, 0, sizeof vb_desc);
           
    vb_desc.size         = sizeof MyVertex * VERTICES_COUNT;
    vb_desc.usage_mode   = UsageMode_Default;
    vb_desc.bind_flags   = BindFlag_VertexBuffer;
    vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;
    
    BufferPtr vb (test.device->CreateBuffer (vb_desc), false);
    
    static const MyVertex verts [] = {
      {{-1, -1, 0}, {0, 0, 1}, {255, 0, 0, 0}},
      {{ 1, -1, 0}, {0, 0, 1}, {0, 255, 0, 0}},
      {{ 0, 1, 0}, {0, 0, 1}, {0, 0, 255, 0}},
    };
    
    vb->SetData (0, vb_desc.size, verts);
    
    printf ("Set input-stage\n");
    
    VertexAttribute attributes [] = {
      {VertexAttributeSemantic_Normal, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, normal), sizeof (MyVertex)},
      {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, position), sizeof (MyVertex)},
      {VertexAttributeSemantic_Color, InputDataFormat_Vector4, InputDataType_Float, 0, offsetof (MyVertex, color), sizeof (MyVertex)},
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
      {"v_shader", size_t (-1), vertex_shader_source.c_str (), "glsl.vs", ""},
      {"p_shader", size_t (-1), pixel_shader_source.c_str (), "glsl.ps", ""},
    };

    static ProgramParameter shader_parameters[] = {
      {"GrainSizeRecip", ProgramParameterType_Float, 0, 1, offsetof (MyShaderParameters, grain_size_recip)},
      {"DarkColor", ProgramParameterType_Float3, 0, 1, offsetof (MyShaderParameters, dark_color)},
      {"colorSpread", ProgramParameterType_Float3, 0, 1, offsetof (MyShaderParameters, color_spread)},
      {"LightPosition", ProgramParameterType_Float3, 0, 1, offsetof (MyShaderParameters, light_position)},
      {"Scale", ProgramParameterType_Float, 0, 1, offsetof (MyShaderParameters, scale)},
      {"Transform", ProgramParameterType_Float4x4, 1, 1, offsetof (MyShaderParameters2, transform)}
    };

    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

    ProgramPtr shader (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print));
    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc));

    BufferDesc cb_desc;
    
    memset (&cb_desc, 0, sizeof cb_desc);
    
    cb_desc.size         = sizeof MyShaderParameters;
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    BufferDesc cb_desc2 (cb_desc);
   
    cb_desc2.size = sizeof MyShaderParameters2;
 
    BufferPtr cb (test.device->CreateBuffer (cb_desc), false), cb2 (test.device->CreateBuffer (cb_desc2), false);

    MyShaderParameters my_shader_parameters = {
      1.0f,
      math::vec3f (0.6f, 0.3f, 0.1f),
      math::vec3f (0.15f, 0.15f / 2.0f, 0),
      math::vec3f (0.0f, 0.0f, 4.0f),
      1.f
    };

    MyShaderParameters2 my_shader_parameters2 = {1.0f};

    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);
    cb2->SetData (0, sizeof my_shader_parameters2, &my_shader_parameters2);

    test.device->SSSetProgram (shader.get ());
    test.device->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test.device->SSSetConstantBuffer (0, cb.get ());
    test.device->SSSetConstantBuffer (1, cb2.get ());
    
    printf ("Set output stage\n");
   
    DepthStencilDesc depth_stencil_desc;

    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));

    depth_stencil_desc.depth_test_enable   = true; //???
    depth_stencil_desc.stencil_test_enable = false;
    depth_stencil_desc.depth_compare_mode  = CompareMode_Less;
    depth_stencil_desc.depth_write_enable  = false;

    DepthStencilStatePtr depth_stencil_state (test.device->CreateDepthStencilState (depth_stencil_desc), false);
    
    test.device->OSSetDepthStencilState (depth_stencil_state.get ());

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
