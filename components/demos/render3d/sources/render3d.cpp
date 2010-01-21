#include "shared.h"

const char*  VERTEX_SHADER_FILE_NAME  = "data/phong.vert";
const char*  PIXEL_SHADER_FILE_NAME   = "data/phong.frag";
const char*  SHADERS_DIR              = "data";
const char*  CONFIG_NAME              = "data/config.xml";
const char*  MODEL_NAME               = "data/meshes.xmesh";
const char*  ENTERPRISE_MODEL_NAME    = "data/enterprise_meshes.xmesh";
const char*  MATERIAL_LIBRARIES []    = {"data/materials.xmtl", "data/custom.xmtl", "data/enterprise_materials.xmtl"};
const char*  SCENE_NAME               = "data/scene.xscene";
const char* ENTERPRISE_SCENE_NAME     = "data/enterprise_scene.xscene";
const char*  REFLECTION_TEXTURE       = "env/EnvGala_000_D2.tga";
const char*  SKY_MESH                 = "_SkyMesh";
const char*  SKY_MATERIAL             = "_SkyMaterial";
const size_t SKY_PARALLELS            = 30;
const size_t SKY_MERIDIANS            = 30;
const float  SKY_RADIUS               = 9000;

const float EPS = 0.001f;

void reload_shaders (Test& test)
{
  printf ("Load shaders\n");

  test.shader_manager.ReloadShaders ();
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

  if (test.physics_enabled)
  {
    test.physics_scene->PerformSimulation (dt);

    for (Test::RigidBodiesMap::iterator iter = test.rigid_bodies.begin (), end = test.rigid_bodies.end (); iter != end; ++iter)
    {
      const physics::low_level::Transform& body_transform = iter->second->WorldTransform ();

      iter->first->SetWorldPosition (body_transform.position);
      iter->first->SetWorldOrientation (body_transform.orientation);
    }

//  test.camera_body->SetLinearVelocity (math::vec3f (test.x_camera_speed, 0, test.y_camera_speed) * inverse (test.camera->WorldTM ()));
//  test.camera_body->SetAngularVelocity (math::vec3f (test.y_camera_rotation_speed, test.x_camera_rotation_speed, test.z_camera_rotation_speed) * inverse (test.camera->WorldTM ()));
    test.camera_body->AddForce (math::vec3f (test.x_camera_speed, 0, test.y_camera_speed) * inverse (test.camera->WorldTM ()));
    test.camera_body->AddTorque (math::vec3f (test.y_camera_rotation_speed / 20.f, test.x_camera_rotation_speed / 20.f, test.z_camera_rotation_speed / 20.f) * inverse (test.camera->WorldTM ()));
  }
  else
  {
    if (fabs (test.x_camera_speed) > EPS || fabs (test.y_camera_speed) > EPS)
      test.camera->Translate (math::vec3f (dt * test.x_camera_speed, 0.f, dt * test.y_camera_speed), NodeTransformSpace_Local);
    if (fabs (test.x_camera_rotation_speed) > EPS || fabs (test.y_camera_rotation_speed) > EPS || fabs (test.z_camera_rotation_speed) > EPS)
      test.camera->Rotate (math::degree (dt * test.y_camera_rotation_speed), math::degree (dt * test.x_camera_rotation_speed), math::degree (dt * test.z_camera_rotation_speed), NodeTransformSpace_Local);
  }

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
  
  my_shader_parameters.light_pos = test.light->WorldPosition ();
  my_shader_parameters.light_dir = test.light->WorldOrtZ ();

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
    
    printf ("Setup rasterizer stage\n");
    
    RasterizerDesc rasterizer_desc;
    
    memset (&rasterizer_desc, 0, sizeof (rasterizer_desc));
    
    rasterizer_desc.fill_mode               = FillMode_Solid;
    rasterizer_desc.cull_mode               = CullMode_None;
    rasterizer_desc.front_counter_clockwise = true;
    rasterizer_desc.depth_bias              = 0;
    rasterizer_desc.scissor_enable          = false;
    rasterizer_desc.multisample_enable      = false;
    rasterizer_desc.antialiased_line_enable = false;
    
    RasterizerStatePtr rasterizer (test.device->CreateRasterizerState (rasterizer_desc), false);

    test.device->RSSetState (rasterizer.get ());
    
    printf ("Setup output stage\n");
    
/*    BlendDesc blend_desc;

    memset (&blend_desc, 0, sizeof (blend_desc));

    blend_desc.blend_enable                     = true;
    blend_desc.blend_color_operation            = BlendOperation_Add;
    blend_desc.blend_alpha_operation            = BlendOperation_Add;
    blend_desc.blend_color_source_argument      = BlendArgument_SourceAlpha;
    blend_desc.blend_color_destination_argument = BlendArgument_InverseSourceAlpha;
    blend_desc.blend_alpha_source_argument      = BlendArgument_SourceAlpha;
    blend_desc.blend_alpha_destination_argument = BlendArgument_InverseSourceAlpha;
    blend_desc.color_write_mask                 = ColorWriteFlag_All;
    
    BlendStatePtr blend_state (test.device->CreateBlendState (blend_desc), false);
    
    test.device->OSSetBlendState (blend_state.get ());*/
    
    printf ("Setup shader stage\n");
    
    test.shader_manager.SetShadersDir (SHADERS_DIR);
    
    reload_shaders (test);

    static ProgramParameter shader_parameters[] = {
      {"ModelViewProjectionMatrix", ProgramParameterType_Float4x4, ConstantBufferSemantic_Transformations, 1, TEST_OFFSETOF (TransformationsShaderParams, model_view_proj_tm)},
      {"ModelViewMatrix", ProgramParameterType_Float4x4, ConstantBufferSemantic_Transformations, 1, TEST_OFFSETOF (TransformationsShaderParams, model_view_tm)},
      {"ViewMatrix", ProgramParameterType_Float4x4, ConstantBufferSemantic_Transformations, 1, TEST_OFFSETOF (TransformationsShaderParams, view_tm)},      

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
    
    for (size_t i=0; i<sizeof (MATERIAL_LIBRARIES) / sizeof (*MATERIAL_LIBRARIES); i++)
      test.material_manager.LoadMaterials (MATERIAL_LIBRARIES [i]);
    
    printf ("Load meshes\n");
    
    test.mesh_manager.LoadMeshes (MODEL_NAME);
    test.mesh_manager.LoadMeshes (ENTERPRISE_MODEL_NAME);
    
    printf ("Create custom meshes\n");
    
    test.mesh_manager.RegisterMesh (create_sphere (SKY_MESH, *test.device, SKY_PARALLELS, SKY_MERIDIANS, 
    test.material_manager.FindMaterial (SKY_MATERIAL)));

    printf ("Load scene\n");
    Node::Pointer main_ship = test.scene_manager.LoadScene (SCENE_NAME);

//    main_ship->Rotate (math::degree (10.f), math::degree (180.f), math::degree (0.f));
//    main_ship->Translate (0, -5, 25);

//    main_ship->BindToParent (*test.camera);
    
    printf ("Add SkyBox\n");
    
    scene_graph::VisualModel::Pointer sky = scene_graph::VisualModel::Create ();

    sky->SetMeshName           (SKY_MESH);
    sky->SetScale              (math::vec3f (SKY_RADIUS));
    sky->SetOrientationInherit (false);
    sky->SetScaleInherit       (false);    
    sky->BindToParent          (*test.camera, scene_graph::NodeBindMode_AddRef);

    printf ("Load enterprise scene\n");

    for (size_t i = 0; i < 10; i++)
    {
      Node::Pointer enterprise = test.scene_manager.LoadScene (ENTERPRISE_SCENE_NAME);

      enterprise->Translate ((float)(rand () % 75 - 30), (float)(rand () % 75 - 30), (float)(rand () % 75 - 30));

      xtl::com_ptr<physics::low_level::IShape> sphere_shape = test.physics_driver->CreateSphereShape (8.f);

      RigidBodyPtr enterprise_body (test.physics_scene->CreateRigidBody (sphere_shape.get (), 1), false);

      physics::low_level::Transform enterprise_transform;

      enterprise_transform.position    = enterprise->WorldPosition ();
      enterprise_transform.orientation = enterprise->WorldOrientation ();

      enterprise_body->SetWorldTransform (enterprise_transform);

      test.rigid_bodies.insert_pair (enterprise, enterprise_body);
    }

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
