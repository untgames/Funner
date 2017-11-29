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
  float       grain_size_recip;
  math::vec3f dark_color;
  math::vec3f color_spread;
  math::vec3f light_position;
  float       scale;
};

struct MyShaderParameters2
{
  math::mat4f transform;
};

class DrawApplication
{
  TestPtr                    test;
  BufferPtr                  vb;
  BufferPtr                  cb;
  BufferPtr                  cb2;
  InputLayoutPtr             layout;
  ProgramPtr                 shader;
  ProgramParametersLayoutPtr program_parameters_layout;

  static void Print (const char* message)
  {
    printf ("Shader message: '%s'\n", message);
  }

  static void Redraw (Test& test)
  {
    test.device->GetImmediateContext ()->Draw (PrimitiveType_TriangleList, 0, 3);

    PredicatePtr predicate (test.device->CreatePredicate (), false);
    
    predicate->Begin (test.device->GetImmediateContext ());

    test.device->GetImmediateContext ()->Draw (PrimitiveType_TriangleList, 3, 3);

    predicate->End (test.device->GetImmediateContext ());

    test.device->GetImmediateContext ()->SetPredication (predicate.get (), true);

    test.device->GetImmediateContext ()->Flush ();

    syslib::Application::Sleep (10);

    test.device->GetImmediateContext ()->Draw (PrimitiveType_TriangleList, 6, 3);
  }

  void OnInitialize ()
  {
    try
    {
      test = new Test (L"OpenGL device test window (draw4)", &Redraw);

      test->window.Show ();

      CreateVertexBuffer ();
      SetInputStage ();
      SetShaderStage ();
    }
    catch (const xtl::exception& e)
    {
      printf("%s failed: %s\n", __FUNCTION__, e.what());
      syslib::Application::Exit (1);
    }
  }

  void OnExit ()
  {
    // Test object should be destroyed
    // BEFORE application exit.
    test.reset ();
  }

  void CreateVertexBuffer ()
  {
    try
    {
      printf ("Create vertex buffer\n");
      
      static const size_t VERTICES_COUNT = 9;
      
      BufferDesc vb_desc;
      
      memset (&vb_desc, 0, sizeof vb_desc);
             
      vb_desc.size         = sizeof (MyVertex) * VERTICES_COUNT;
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;
      
      vb = BufferPtr (test->device->CreateBuffer (vb_desc), false);
      
      static const MyVertex verts [] = {
        {{-1, -1, 0}, {0, 0, 1}, {255, 0, 0, 0}},
        {{-1,  1, 0}, {0, 0, 1}, {0, 255, 0, 0}},
        {{ 1, -1, 0}, {0, 0, 1}, {0, 0, 255, 0}},

        {{-1, -1, 0.5}, {0, 0, 1}, {255, 0, 0, 0}},
        {{-1,  0, 0.5}, {0, 0, 1}, {0, 255, 0, 0}},
        {{ 0, -1, 0.5}, {0, 0, 1}, {0, 0, 255, 0}},

        {{1, 1, 0}, {0, 0, 1}, {255, 0, 0, 0}},
        {{0, 1, 0}, {0, 0, 1}, {0, 255, 0, 0}},
        {{1, 0, 0}, {0, 0, 1}, {0, 0, 255, 0}},
      };
      
      vb->SetData (0, vb_desc.size, verts);
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

  void SetInputStage ()
  {
    try
    {
      printf ("Set input-stage\n");
      
      VertexAttribute attributes [] = {
        {
            test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Normal)
          , InputDataFormat_Vector3
          , InputDataType_Float
          , 0
          , TEST_OFFSETOF (MyVertex, normal)
          , sizeof (MyVertex)
        },
        {
            test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Position)
          , InputDataFormat_Vector3
          , InputDataType_Float
          , 0
          , TEST_OFFSETOF (MyVertex, position)
          , sizeof (MyVertex)
        },
        {
            test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Color)
          , InputDataFormat_Vector4
          , InputDataType_Float
          , 0
          , TEST_OFFSETOF (MyVertex, color)
          , sizeof (MyVertex)
        },
      };
      
      InputLayoutDesc layout_desc;
      
      memset (&layout_desc, 0, sizeof layout_desc);
      
      layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
      layout_desc.vertex_attributes       = attributes;
      layout_desc.index_type              = InputDataType_UInt;
      layout_desc.index_buffer_offset     = 0;
      
      layout = InputLayoutPtr (test->device->CreateInputLayout (layout_desc), false);

      test->device->GetImmediateContext ()->ISSetInputLayout (layout.get ());
      test->device->GetImmediateContext ()->ISSetVertexBuffer (0, vb.get ());
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }

  }

  void SetShaderStage ()
  {
    try
    {
      printf ("Set shader stage\n");
      
      stl::string pixel_shader_source  = read_shader (PIXEL_SHADER_FILE_NAME),
                  vertex_shader_source = read_shader (VERTEX_SHADER_FILE_NAME);
      
      ShaderDesc shader_descs [] = {
        {"p_shader", (unsigned int)-1, pixel_shader_source.c_str (), "glsl.ps", ""},
        {"v_shader", (unsigned int)-1, vertex_shader_source.c_str (), "glsl.vs", ""}
      };

      static ProgramParameter shader_parameters[] = {
        {"GrainSizeRecip", ProgramParameterType_Float, 0, 1, TEST_OFFSETOF (MyShaderParameters, grain_size_recip)},
        {"DarkColor", ProgramParameterType_Float3, 0, 1, TEST_OFFSETOF (MyShaderParameters, dark_color)},
        {"colorSpread", ProgramParameterType_Float3, 0, 1, TEST_OFFSETOF (MyShaderParameters, color_spread)},
        {"LightPosition", ProgramParameterType_Float3, 0, 1, TEST_OFFSETOF (MyShaderParameters, light_position)},
        {"Scale", ProgramParameterType_Float, 0, 1, TEST_OFFSETOF (MyShaderParameters, scale)},
        {"Transform", ProgramParameterType_Float4x4, 1, 1, TEST_OFFSETOF (MyShaderParameters2, transform)}
      };

      ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

      shader = ProgramPtr (test->device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &DrawApplication::Print), false);
      program_parameters_layout = ProgramParametersLayoutPtr (test->device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

      BufferDesc cb_desc;
      
      memset (&cb_desc, 0, sizeof (cb_desc));
      
      cb_desc.size         = sizeof (MyShaderParameters);
      cb_desc.usage_mode   = UsageMode_Default;
      cb_desc.bind_flags   = BindFlag_ConstantBuffer;
      cb_desc.access_flags = AccessFlag_ReadWrite;

      BufferDesc cb_desc2 (cb_desc);
     
      cb_desc2.size = sizeof (MyShaderParameters2);
   
      cb  = BufferPtr (test->device->CreateBuffer (cb_desc), false);
      cb2 = BufferPtr (test->device->CreateBuffer (cb_desc2), false);

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

      test->device->GetImmediateContext ()->SSSetProgram (shader.get ());
      test->device->GetImmediateContext ()->SSSetProgramParametersLayout (program_parameters_layout.get ());
      test->device->GetImmediateContext ()->SSSetConstantBuffer (0, cb.get ());
      test->device->GetImmediateContext ()->SSSetConstantBuffer (1, cb2.get ());
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

public:
  DrawApplication ()
  {
    syslib::Application::RegisterEventHandler (
        syslib::ApplicationEvent_OnInitialize
      , xtl::bind(&DrawApplication::OnInitialize, this)
    );

    syslib::Application::RegisterEventHandler (
        syslib::ApplicationEvent_OnExit
      , xtl::bind(&DrawApplication::OnExit, this)
    );
  }

  void Run ()
  {
    printf ("Main loop\n");

    syslib::Application::Run ();
  }
};

int main ()
{
  printf ("Results of draw4_test:\n");
  
  try
  {
    DrawApplication app;

    app.Run();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }

  return 0;
}
