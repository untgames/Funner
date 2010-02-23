#include "shared.h"

const char*  SHADERS_DIR              = "data/shaders";
const char*  CONFIG_NAME              = "config.xml";
const char*  MODEL_NAME               = "data/meshes/main_ship.binmesh";
const char*  ENTERPRISE_MODEL_NAME    = "data/meshes/ship_01a.binmesh";
const char*  MATERIAL_LIBRARIES []    = { "data/materials/main_ship.xmtl", "data/materials/ship_01a.xmtl", "data/materials/sky.xmtl", "data/materials/particles.xmtl" };
const char*  PARTICLE_SYSTEMS []      = { "data/particle_systems/particles.xml" };
//const char*  SCENE_NAME               = "data/scenes/main_ship.xscene";
const char*  SCENE_NAME               = "data/scenes/ship_01a.xscene";
const char*  ENTERPRISE_SCENE_NAME    = "data/scenes/ship_01a.xscene";
const char*  REFLECTION_TEXTURE       = "data/textures/environment/EnvGala_000_D.tga";
const char*  SKY_MESH                 = "_SkyMesh";
const char*  SKY_MATERIAL             = "_SkyMaterial";
const int    ADDITIONAL_SHIPS_RANGE   = 200;
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

//  test.camera_body->SetLinearVelocity (math::vec3f (test.x_camera_speed, 0, test.y_camera_speed) * inverse (test.camera->WorldTM ()));
//  test.camera_body->SetAngularVelocity (math::vec3f (test.y_camera_rotation_speed, test.x_camera_rotation_speed, test.z_camera_rotation_speed) * inverse (test.camera->WorldTM ()));
    test.camera_body->AddForce (math::vec3f (test.x_camera_speed, 0, test.y_camera_speed) * inverse (test.camera->WorldTM ()));

    math::quatf rotation = math::to_quat (math::radian (test.y_camera_rotation_speed / 20.f), math::radian (test.x_camera_rotation_speed / 20.f), math::radian (test.z_camera_rotation_speed / 20.f));

    math::quatf camera_rotation = test.camera->WorldOrientation () * rotation * inverse (test.camera->WorldOrientation ());
    math::anglef pitch, yaw, roll;

    to_euler_angles (camera_rotation, pitch, yaw, roll);

    test.camera_body->AddTorque (math::vec3f (radian (pitch), radian (yaw), radian (roll)));


//    test.camera_body->AddTorque (math::vec3f (test.y_camera_rotation_speed / 20.f, test.x_camera_rotation_speed / 20.f, test.z_camera_rotation_speed / 20.f));
    //test.camera_body->AddTorque (math::vec3f (test.y_camera_rotation_speed / 20.f, test.x_camera_rotation_speed / 20.f, test.z_camera_rotation_speed / 20.f) * inverse (test.camera->WorldTM ()));

    test.physics_scene->PerformSimulation (dt);

    for (Test::PhysBodiesMap::iterator iter = test.physics_bodies.begin (), end = test.physics_bodies.end (); iter != end; ++iter)
    {
      const physics::low_level::Transform& body_transform = iter->second->rigid_body->WorldTransform ();

      //if (iter->second->rigid_body != test.camera_body)
        iter->first->SetWorldOrientation (body_transform.orientation);
      iter->first->SetWorldPosition (body_transform.position);

//      iter->first->Translate (math::vec3f (dt, dt, dt), NodeTransformSpace_World);
      
      math::vec3f p = iter->first->WorldPosition ();
          
      printf ("position: %.3f %.3f %.3f ||| %.3f %.3f %.3f\n", p.x, p.y, p.z, body_transform.position.x, body_transform.position.y, body_transform.position.z);
    }
  }
  else
  {
    if (fabs (test.x_camera_speed) > EPS || fabs (test.y_camera_speed) > EPS)
      test.camera->Translate (math::vec3f (dt * test.x_camera_speed, 0.f, dt * test.y_camera_speed), NodeTransformSpace_Local);
    if (fabs (test.x_camera_rotation_speed) > EPS || fabs (test.y_camera_rotation_speed) > EPS || fabs (test.z_camera_rotation_speed) > EPS)
      test.camera->Rotate (math::degree (dt * test.y_camera_rotation_speed), math::degree (dt * test.x_camera_rotation_speed), math::degree (dt * test.z_camera_rotation_speed), NodeTransformSpace_Local);
  }

  test.scene_manager.Scene ().Root ().Update (dt);

  if (current_time - last_fps > 1000)
  {
    printf ("FPS: %.2f\n", float (frames_count) / float (current_time - last_fps) * 1000.f);

    last_fps = current_time;
    frames_count = 0;
    return;
  }

  test.window.Invalidate ();
}

Test* global_test = 0;

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
    
    printf ("Initialize renderer\n");
    
    test.scene_renderer.InitializeResources ();
    
    printf ("Load particle systems\n");

    for (size_t i = 0; i < sizeof (PARTICLE_SYSTEMS) / sizeof (*PARTICLE_SYSTEMS); i++)
      test.particle_systems_manager.LoadParticleSystems (PARTICLE_SYSTEMS [i]);

    printf ("Load shaders\n");
      
    test.shader_manager.SetShadersDir (SHADERS_DIR);
  
    reload_shaders (test);    
    
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

    printf ("Add SkyBox\n");
    
    scene_graph::VisualModel::Pointer sky = scene_graph::VisualModel::Create ();

    sky->SetMeshName           (SKY_MESH);
    sky->SetScale              (math::vec3f (SKY_RADIUS));
    sky->SetOrientationInherit (false);
    sky->SetScaleInherit       (false);    
    sky->BindToParent          (*test.camera, scene_graph::NodeBindMode_AddRef);

    printf ("Load enterprise scene\n");

    for (size_t i = 0; i < 3; i++)
    {
      Node::Pointer enterprise_subnode = test.scene_manager.LoadScene (i ? ENTERPRISE_SCENE_NAME : SCENE_NAME);

      VertexArray verts;
      
      build_vertices (*enterprise_subnode, test, verts);
      
      math::vec3f center;
      float       radius = 1.0f;
      
      build_sphere (verts, center, radius);
      
      printf ("model: [%.3f %.3f %.3f] radius=%.3f\n", center.x, center.y, center.z, radius);
      
      Node::Pointer enterprise = Node::Create ();

      Node::Pointer particle_system = test.particle_systems_manager.CreateParticleSystem ("ship_trail", test.scene_manager.Scene ().Root ());

      particle_system->BindToParent (*enterprise, NodeBindMode_AddRef);

      enterprise_subnode->SetPosition  (-center);
      enterprise_subnode->BindToParent (*enterprise, NodeBindMode_AddRef);
      enterprise->BindToScene          (test.scene_manager.Scene (), NodeBindMode_AddRef);

      enterprise->Translate ((float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2.f),
                             (float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2.f),
                             (float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2.f));

      PhysBodyPtr phys_body (new PhysBody, false);
      xtl::com_ptr<physics::low_level::IShape> shape (test.physics_driver->CreateSphereShape (radius), false);
//      xtl::com_ptr<physics::low_level::IShape> shape (test.physics_driver->CreateConvexShape (verts.size (), &verts [0]), false);

      phys_body->rigid_body = RigidBodyPtr (test.physics_scene->CreateRigidBody (shape.get (), 1), false);

      physics::low_level::Transform enterprise_transform;

      enterprise_transform.position    = enterprise->WorldPosition ();
      enterprise_transform.orientation = enterprise->WorldOrientation ();      

      phys_body->rigid_body->SetWorldTransform (enterprise_transform);

      test.physics_bodies.insert_pair (enterprise, phys_body);

      if (i)
      {
        test.scene_manager.AddShattle (enterprise);
        enterprise->AttachController (EnemyAi (test, *enterprise, *phys_body->rigid_body));
      }
      else
      {
        test.scene_manager.AddMainShip (enterprise);
      }
    }

    test.scene_manager.SetDrawMainShips (false);

    printf ("Register callbacks\n");

    global_test = &test;

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
