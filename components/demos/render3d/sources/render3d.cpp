#include "shared.h"

const char* VERTEX_SHADER_FILE_NAME  = "data/phong.vert";
const char* PIXEL_SHADER_FILE_NAME   = "data/phong.frag";
const char* CONFIG_NAME              = "data/config.xml";
const char* MODEL_NAME               = "data/meshes.xmesh";
const char* MATERIAL_LIBRARY         = "data/materials.xmtl";
const char* SCENE_NAME               = "data/scene.xscene";
const char* REFLECTION_TEXTURE       = "env/EnvGala_000_D2.tga";

const float EPS = 0.001f;

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

void reload_shaders (Test& test)
{
  printf ("Load shaders\n");

  stl::string vertex_shader_source = read_shader (VERTEX_SHADER_FILE_NAME),
              pixel_shader_source  = read_shader (PIXEL_SHADER_FILE_NAME);

  ShaderDesc shader_descs [] = {
    {VERTEX_SHADER_FILE_NAME, size_t (-1), vertex_shader_source.c_str (), "glsl.vs", ""},
    {PIXEL_SHADER_FILE_NAME, size_t (-1), pixel_shader_source.c_str (), "glsl.ps", ""},
  };

  ProgramPtr shader (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print), false);
  
  test.device->SSSetProgram (shader.get ());

  test.shader = shader;
}

void reload (Test& test)
{
  reload_shaders (test);
}

size_t frames_count = 0;

void redraw (Test& test)
{
  test.scene_renderer.Draw (*test.camera);

  frames_count++;
}

void idle (Test& test)
{
  if (test.window.IsClosed ())
    return;

  static size_t last     = 0;
  static size_t last_fps = 0;

  size_t current_time = common::milliseconds ();

  float dt = (current_time - last) / 1000.f;

  last = current_time;

  if (fabs (test.x_camera_speed) > EPS || fabs (test.y_camera_speed) > EPS)
    test.camera->Translate (math::vec3f (dt * test.x_camera_speed, 0.f, dt * test.y_camera_speed), NodeTransformSpace_Local);
  if (fabs (test.x_camera_rotation_speed) > EPS || fabs (test.y_camera_rotation_speed) > EPS)
    test.camera->Rotate (math::degree (dt * test.y_camera_rotation_speed), math::degree (dt * test.x_camera_rotation_speed), math::degree (0.f));

  if (current_time - last_fps > 1000)
  {
    printf ("FPS: %.2f\n", float (frames_count) / float (current_time - last_fps) * 1000.f);

    last_fps = current_time;
    frames_count = 0;
    return;
  }

  CommonShaderParams my_shader_parameters;

  IBuffer* cb = test.device->SSGetConstantBuffer (0);

  if (!cb)
  {
    printf ("Null constant buffer #0\n");
    return;
  }

  cb->GetData (0, sizeof my_shader_parameters, &my_shader_parameters);
  
  static float light_phi = -3.14f / 2.f;

  my_shader_parameters.light_pos = math::vec3f (10 * cos (light_phi), 0.f, 10 * sin (light_phi));
  my_shader_parameters.light_dir = -normalize (my_shader_parameters.light_pos);

  cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

  test.window.Invalidate ();
}

int main ()
{
  printf ("Results of model_load_test:\n");

  try
  {
    printf ("Load configuration\n");
    
    common::Parser    p (CONFIG_NAME);
    common::ParseLog  log         = p.Log ();
    common::ParseNode config_root = p.Root ().First ("Config");

    for (size_t i = 0; i < log.MessagesCount (); i++)
      switch (log.MessageType (i))
      {
        case common::ParseLogMessageType_Error:
        case common::ParseLogMessageType_FatalError:
          throw xtl::format_operation_exception ("LoadScene", log.Message (i));
        default:
          break;
      }
      
    printf ("Initialize application\n");      

    Test test (L"OpenGL device test window (model_load)", &redraw, &reload, common::get<const char*> (config_root, "AdapterMask", "*"), common::get<const char*> (config_root, "InitString", ""));

    test.window.SetSize (common::get<size_t> (config_root, "WindowWidth", 800), common::get<size_t> (config_root, "WindowHeight", 600));

    test.window.Show ();
    
    printf ("Setup shader stage\n");
    
    reload_shaders (test);

    static ProgramParameter shader_parameters[] = {
      {"ModelViewProjectionMatrix", ProgramParameterType_Float4x4, ConstantBufferSemantic_Transformations, 1, TEST_OFFSETOF (TransformationsShaderParams, model_view_proj_tm)},
      {"ModelViewMatrix", ProgramParameterType_Float4x4, ConstantBufferSemantic_Transformations, 1, TEST_OFFSETOF (TransformationsShaderParams, model_view_tm)},

      {"LightPosition", ProgramParameterType_Float3, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, light_pos)},
      {"LightDirection", ProgramParameterType_Float3, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, light_dir)},
      {"BumpTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, bump_sampler)},
      {"DiffuseTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, diffuse_sampler)},
      {"AmbientTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, ambient_sampler)},
      {"SpecularTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, specular_sampler)},
      {"EmissionTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, emission_sampler)},
      {"ReflectionTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, reflection_sampler)},      
      
      {"ShaderType", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, shader_type)},
      {"Reflectivity", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, reflectivity)},
      {"Transparency", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, transparency)},   
      {"Shininess", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, shininess)},
      {"BumpAmount", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, bump_amount)},
      {"BumpTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, bump_texture_channel)},
      {"DiffuseTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, diffuse_texture_channel)},
      {"AmbientTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, ambient_texture_channel)},
      {"SpecularTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, specular_texture_channel)},
      {"EmissionTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, emission_texture_channel)},
      {"DiffuseColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, diffuse_color)},
      {"AmbientColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, ambient_color)},
      {"SpecularColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, specular_color)},
      {"EmissionColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, emission_color)},
    };
    
    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};
    
    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

    BufferDesc cb_desc;

    memset (&cb_desc, 0, sizeof cb_desc);

    cb_desc.size         = sizeof (CommonShaderParams);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    BufferPtr common_cb (test.device->CreateBuffer (cb_desc), false);

    CommonShaderParams common_shader_params;
    
    common_shader_params.bump_sampler       = SamplerChannel_Bump;
    common_shader_params.diffuse_sampler    = SamplerChannel_Diffuse;
    common_shader_params.specular_sampler   = SamplerChannel_Specular;
    common_shader_params.ambient_sampler    = SamplerChannel_Ambient;
    common_shader_params.emission_sampler   = SamplerChannel_Emission;
    common_shader_params.reflection_sampler = SamplerChannel_Reflection;
    
    common_cb->SetData (0, sizeof common_shader_params, &common_shader_params);

    test.device->SSSetProgramParametersLayout (program_parameters_layout.get ());        
    test.device->SSSetConstantBuffer (ConstantBufferSemantic_Common, common_cb.get ());    
    
    memset (&cb_desc, 0, sizeof cb_desc);

    cb_desc.size         = sizeof (TransformationsShaderParams);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    BufferPtr transformations_cb = BufferPtr (test.device->CreateBuffer (cb_desc), false);

    test.device->SSSetConstantBuffer (ConstantBufferSemantic_Transformations, transformations_cb.get ());
    
    printf ("Load materials\n");
    
    test.material_manager.SetReflectionTexture (REFLECTION_TEXTURE);
    
    test.material_manager.LoadMaterials (MATERIAL_LIBRARY);
    
    printf ("Load meshes\n");
    
    test.mesh_manager.LoadMeshes (MODEL_NAME);

    printf ("Load scene\n");

    test.scene_manager.LoadScene (SCENE_NAME);

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
