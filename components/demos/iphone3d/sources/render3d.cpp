//#include <CFBundle.h>

#include <unistd.h>

#include "shared.h"

const char*  SHADERS_DIR              = "data";
const char*  CONFIG_NAME              = "data/config.xml";
const char*  MODEL_NAME               = "data/meshes.xmesh";
const char*  ENTERPRISE_MODEL_NAME    = "data/enterprise_meshes.xmesh";
const char*  MATERIAL_LIBRARIES []    = {"data/materials.xmtl", "data/enterprise_materials.xmtl"};
const char*  SCENE_NAME               = "data/scene.xscene";
const char* ENTERPRISE_SCENE_NAME     = "data/enterprise_scene.xscene";
const char*  REFLECTION_TEXTURE       = "env/EnvGala_000_D.tga";
const size_t ADDITIONAL_SHIPS_RANGE   = 70;
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

  MyShaderParameters my_shader_parameters;

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

Test *test;
RasterizerStatePtr rasterizer;
ProgramParametersLayoutPtr program_parameters_layout;
BufferPtr common_cb;
BufferPtr transformations_cb;
Node::Pointer main_ship;

void start_handler ()
{
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

    test = new Test (L"OpenGL device test window (model_load)", &redraw, &reload, common::get<const char*> (config_root, "AdapterMask", "*"), common::get<const char*> (config_root, "InitString", ""));

    test->window.SetSize (common::get<size_t> (config_root, "WindowWidth", 800), common::get<size_t> (config_root, "WindowHeight", 600));

    test->window.Show ();
    
    printf ("Setup rasterizer stage\n");
    
    RasterizerDesc rasterizer_desc;
    
    memset (&rasterizer_desc, 0, sizeof (rasterizer_desc));
    
    rasterizer_desc.fill_mode               = FillMode_Solid;
    rasterizer_desc.cull_mode               = CullMode_None;
    rasterizer_desc.front_counter_clockwise = true;
    rasterizer_desc.depth_bias              = 0;
    rasterizer_desc.scissor_enable          = false;
    rasterizer_desc.multisample_enable      = true;
    rasterizer_desc.antialiased_line_enable = false;
    
    rasterizer = RasterizerStatePtr (test->device->CreateRasterizerState (rasterizer_desc), false);

    test->device->RSSetState (rasterizer.get ());
    
    printf ("Setup output stage\n");
    
    printf ("Setup shader stage\n");
    
    test->shader_manager.SetShadersDir (SHADERS_DIR);
    
    reload_shaders (*test);

    static ProgramParameter shader_parameters[] = {
       {"myProjMatrix", ProgramParameterType_Float4x4, 0, 1, TEST_OFFSETOF (MyShaderParameters, proj_tm)},
       {"myViewMatrix", ProgramParameterType_Float4x4, 0, 1, TEST_OFFSETOF (MyShaderParameters, view_tm)},
       {"myObjectMatrix", ProgramParameterType_Float4x4, 0, 1, TEST_OFFSETOF (MyShaderParameters, object_tm)},
       {"lightPos", ProgramParameterType_Float3, 0, 1, TEST_OFFSETOF (MyShaderParameters, light_pos)},
       {"lightDir", ProgramParameterType_Float3, 0, 1, TEST_OFFSETOF (MyShaderParameters, light_dir)},
    };
    
    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};
    
    program_parameters_layout = ProgramParametersLayoutPtr (test->device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

    BufferDesc cb_desc;

    memset (&cb_desc, 0, sizeof cb_desc);

    cb_desc.size         = sizeof (MyShaderParameters);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    common_cb = BufferPtr (test->device->CreateBuffer (cb_desc), false);

    MyShaderParameters my_shader_parameters;
    
    my_shader_parameters.view_tm = inverse (math::lookat (math::vec3f (0, 400, 0), math::vec3f (0.0f), math::vec3f (0, 0, 1)));

    common_cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

    test->device->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test->device->SSSetConstantBuffer (ConstantBufferSemantic_Common, common_cb.get ());
    
    printf ("Load materials\n");
    
    test->material_manager.SetReflectionTexture (REFLECTION_TEXTURE);
    
    for (size_t i=0; i<sizeof (MATERIAL_LIBRARIES) / sizeof (*MATERIAL_LIBRARIES); i++)
      test->material_manager.LoadMaterials (MATERIAL_LIBRARIES [i]);
    
    printf ("Load meshes\n");
    
    test->mesh_manager.LoadMeshes (MODEL_NAME);
    test->mesh_manager.LoadMeshes (ENTERPRISE_MODEL_NAME);
    
    printf ("Load scene\n");
    main_ship = test->scene_manager.LoadScene (SCENE_NAME);

//    main_ship->Rotate (math::degree (10.f), math::degree (180.f), math::degree (0.f));
//    main_ship->Translate (0, -5, 25);

//    main_ship->BindToParent (*test->camera);

    printf ("Register callbacks\n");

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (*test)));
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}

int main ()
{
/*  printf ("Results of model_load_test:\n");

  CFBundleRef main_bundle = CFBundleGetMainBundle ();

  if (!main_bundle)
  {
    printf ("Can't locate main bundle\n");
    return 1;
  }

  CFURLRef main_bundle_url = CFBundleCopyBundleURL (main_bundle);

  if (!main_bundle_url)
  {
    printf ("Can't get main bundle url\n");
    return 1;
  }
  else
  {
    char buf [16 * 1024];

    CFStringRef filesystem_path = CFURLCopyFileSystemPath (main_bundle_url, kCFURLPOSIXPathStyle);

    CFRelease (main_bundle_url);

    if (!filesystem_path)
    {
      printf ("Can't get filesystem path to bundle\n");
      return 1;
    }

    Boolean get_string_result = CFStringGetCString (filesystem_path, buf, 16 * 1024, kCFStringEncodingASCII);

    CFRelease (filesystem_path);

    if (!get_string_result)
    {
      printf ("Can't get c-string path to bundle\n");
      return 1;
    }

    if (chdir (buf))
    {
      printf ("Can't set working dir '%s'\n", buf);
      return 1;
    }
  }*/

  syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnEnterRunLoop, &start_handler);

  printf ("Main loop\n");

  syslib::Application::Run ();

  return 0;
}
