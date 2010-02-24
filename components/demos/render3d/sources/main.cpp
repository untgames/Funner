#include "shared.h"

const char*  SHADERS_DIR               = "data/shaders";
const char*  CONFIG_NAME               = "config.xml";
const char*  MODEL_NAME                = "data/meshes/main_ship.binmesh";
const char*  ENTERPRISE_MODEL_NAMES [] = {"data/meshes/ship_01a.binmesh", "data/meshes/ship_11a.binmesh"};
const char*  MATERIAL_LIBRARIES []     = { "data/materials/main_ship.xmtl", "data/materials/ship_01a.xmtl", "data/materials/sky.xmtl",
                                          "data/materials/particles.xmtl", "data/materials/gfx.xmtl", "data/materials/ship_11a.xmtl" };
const char*  PARTICLE_SYSTEMS []       = { "data/particle_systems/particles.xml" };
const char*  SCENE_NAME                = "data/scenes/main_ship.xscene";
const char*  ENTERPRISE_SCENE_NAMES [] = {"data/scenes/ship_01a.xscene", "data/scenes/ship_11a.xscene"};
const char*  REFLECTION_TEXTURE        = "data/textures/environment/EnvGala_000_D.tga";
const char*  SKY_MESH                  = "_SkyMesh";
const char*  SKY_MATERIAL              = "_SkyMaterial";
const char*  GUN_NODE_NAME             = "gun";
const int    ADDITIONAL_SHIPS_RANGE    = 200;
const size_t SKY_PARALLELS             = 30;
const size_t SKY_MERIDIANS             = 30;
const float  SKY_RADIUS                = 9000;
const size_t ENTERPRISE_MODELS_COUNT   = sizeof (ENTERPRISE_SCENE_NAMES) / sizeof (*ENTERPRISE_SCENE_NAMES);
const float  SHOT_RATE                = 10;

const math::vec4f PLAYER_SHOT_COLOR (0.f, 0.8f, 1.f, 1.f);
const float       PLAYER_SHOT_DISTANCE = 1000.f;


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
  if (!test.current_camera)
    return;

  test.scene_renderer.Draw (*test.current_camera);

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
    test.camera_body->AddForce (math::vec3f (test.x_camera_speed, 0, test.y_camera_speed) * inverse (test.camera->WorldTM ()));

    math::quatf rotation = math::to_quat (math::radian (test.y_camera_rotation_speed / 20.f), math::radian (test.x_camera_rotation_speed / 20.f), math::radian (test.z_camera_rotation_speed / 20.f));

    math::quatf camera_rotation = test.camera->WorldOrientation () * rotation * inverse (test.camera->WorldOrientation ());
    math::anglef pitch, yaw, roll;

    to_euler_angles (camera_rotation, pitch, yaw, roll);

    test.camera_body->AddTorque (math::vec3f (radian (pitch), radian (yaw), radian (roll)));

    test.physics_scene->PerformSimulation (dt);

    for (Test::PhysBodiesMap::iterator iter = test.physics_bodies.begin (), end = test.physics_bodies.end (); iter != end; ++iter)
    {
      const physics::low_level::Transform& body_transform = iter->second->rigid_body->WorldTransform ();

      iter->first->SetWorldOrientation (body_transform.orientation);
      iter->first->SetWorldPosition (body_transform.position);      
    }
  }
  else
  {
    if (fabs (test.x_camera_speed) > EPS || fabs (test.y_camera_speed) > EPS)
      test.camera->Translate (math::vec3f (dt * test.x_camera_speed, 0.f, dt * test.y_camera_speed), NodeTransformSpace_Local);
    if (fabs (test.x_camera_rotation_speed) > EPS || fabs (test.y_camera_rotation_speed) > EPS || fabs (test.z_camera_rotation_speed) > EPS)
      test.camera->Rotate (math::degree (dt * test.y_camera_rotation_speed), math::degree (dt * test.x_camera_rotation_speed), math::degree (dt * test.z_camera_rotation_speed), NodeTransformSpace_Local);
  }

  if (test.shot_pressed && test.player_ship)
  {
    static size_t last_shot_time = 0;

    if (current_time - last_shot_time > 1000 / SHOT_RATE)
    {
      Node::Pointer gun = test.player_ship->FindChild (GUN_NODE_NAME);

      while (gun)
      {
        if (!xtl::xstrcmp (gun->Name (), GUN_NODE_NAME))
          test.gfx_manager.PerformShot (*gun, PLAYER_SHOT_COLOR, PLAYER_SHOT_DISTANCE);

        gun = gun->NextChild ();
      }

      last_shot_time = current_time;
    }
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
    
    for (int i=0; i<ENTERPRISE_MODELS_COUNT; i++)
      test.mesh_manager.LoadMeshes (ENTERPRISE_MODEL_NAMES [i]);
    
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

    printf ("Load ships\n");

    for (size_t i = 0; i < 10; i++)
    {
      const char* scene_name = i ? ENTERPRISE_SCENE_NAMES [rand () % ENTERPRISE_MODELS_COUNT] : SCENE_NAME;
      
      Node::Pointer ship_subnode = test.scene_manager.LoadScene (scene_name);

      VertexArray verts;
      
      build_vertices (*ship_subnode, test, verts);
      
      math::vec3f center;
      math::vec3f scale = ship_subnode->WorldScale ();
      float       radius = 1.0f;
      
      build_sphere (verts, center, radius);
      
      printf ("model: center=[%.3f %.3f %.3f] radius=%.3f scale=[%.3f %.3f %.3f]\n", center.x, center.y, center.z, radius,
        scale.x, scale.y, scale.z);
        
      for (VertexArray::iterator iter=verts.begin (), end=verts.end (); iter!=end; ++iter)
        *iter -= center;
      
      Node::Pointer ship = Node::Create ();
           
      ship_subnode->SetPosition  (-center);
      ship_subnode->BindToParent (*ship, NodeBindMode_AddRef);
      ship->BindToScene          (test.scene_manager.Scene (), NodeBindMode_AddRef);

      ship->Translate ((float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2.f),
                             (float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2.f),
                             (float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2.f));

      PhysBodyPtr phys_body (new PhysBody, false);
      xtl::com_ptr<physics::low_level::IShape> shape (test.physics_driver->CreateSphereShape (radius), false);
//      xtl::com_ptr<physics::low_level::IShape> shape (test.physics_driver->CreateConvexShape (verts.size (), &verts [0]), false);

      phys_body->rigid_body = RigidBodyPtr (test.physics_scene->CreateRigidBody (shape.get (), 1), false);

      physics::low_level::Transform ship_transform;

      ship_transform.position    = ship->WorldPosition ();
      ship_transform.orientation = ship->WorldOrientation ();      

      phys_body->rigid_body->SetWorldTransform (ship_transform);

      test.physics_bodies.insert_pair (ship, phys_body);

      if (i)
      {
        test.scene_manager.AddShattle (ship);
        
        ship->AttachController (EnemyAi (test, *ship, *phys_body->rigid_body)); 
        
        if (!strcmp (scene_name, ENTERPRISE_SCENE_NAMES [0]))
        {
          Node::Pointer left_gun_node  = Node::Create (),
                        right_gun_node = Node::Create ();

          left_gun_node->SetName (GUN_NODE_NAME);
          left_gun_node->SetPosition    (-4, 0, 3.5);
          left_gun_node->SetOrientation (math::degree (0.f), math::degree (180.f), math::degree (0.f));

          left_gun_node->BindToParent (*ship, NodeBindMode_AddRef);

          right_gun_node->SetName (GUN_NODE_NAME);
          right_gun_node->SetPosition    (4, 0, 3.5);
          right_gun_node->SetOrientation (math::degree (0.f), math::degree (180.f), math::degree (0.f));

          right_gun_node->BindToParent (*ship, NodeBindMode_AddRef);

          Node::Pointer left_particle_system  = test.particle_systems_manager.CreateParticleSystem ("enemy_ship_trail", test.scene_manager.Scene ().Root ()),
                        right_particle_system = test.particle_systems_manager.CreateParticleSystem ("enemy_ship_trail", test.scene_manager.Scene ().Root ());

          left_particle_system->SetPosition (-2.5, -0.5, -4.5);
          left_particle_system->BindToParent (*ship, NodeBindMode_AddRef);
          right_particle_system->SetPosition (2.5, -0.5, -4.5);
          right_particle_system->BindToParent (*ship, NodeBindMode_AddRef);
        }
      }
      else
      {
        Node::Pointer follower = Node::Create ();
        
        test.camera->SetScaleInherit (false);

        follower->BindToScene (test.scene_manager.Scene (), NodeBindMode_AddRef);
        follower->AttachController (FollowNode (*ship, *follower));

        test.camera->SetPosition (0, 10, 20);
        test.camera->Rotate (math::degree (10.0f), math::degree (180.0f), math::degree (0.0f));
//        test.camera->BindToParent (*follower);
        test.camera->BindToParent (*ship);

        test.camera_body = phys_body->rigid_body;
        
        test.inside_camera->SetPosition (0.5, 0.25, -8.5);
        test.inside_camera->Rotate (math::degree (10.0f), math::degree (180.0f), math::degree (0.0f));
        test.inside_camera->BindToParent (*ship);

        test.scene_manager.AddMainShip (ship);                

        Node::Pointer left_gun_node = Node::Create (),
                      right_gun_node = Node::Create ();

        left_gun_node->SetName (GUN_NODE_NAME);
        left_gun_node->SetPosition    (7, -2, -3);
        left_gun_node->SetOrientation (math::degree (0.f), math::degree (180.f), math::degree (0.f));

        left_gun_node->BindToParent (*ship, NodeBindMode_AddRef);

        right_gun_node->SetName (GUN_NODE_NAME);
        right_gun_node->SetPosition    (-6, -2, -3);
        right_gun_node->SetOrientation (math::degree (0.f), math::degree (180.f), math::degree (0.f));

        right_gun_node->BindToParent (*ship, NodeBindMode_AddRef);

        test.player_ship = ship;

        Node::Pointer left_particle_system  = test.particle_systems_manager.CreateParticleSystem ("main_ship_trail", test.scene_manager.Scene ().Root ()),
                      right_particle_system = test.particle_systems_manager.CreateParticleSystem ("main_ship_trail", test.scene_manager.Scene ().Root ());

        left_particle_system->SetPosition (-1.8, 0.f, 7.2);
        left_particle_system->BindToParent (*ship, NodeBindMode_AddRef);
        right_particle_system->SetPosition (2.8, 0.f, 7.2);
        right_particle_system->BindToParent (*ship, NodeBindMode_AddRef);
      }
    }

    test.camera_body->Material ()->SetLinearDamping (0.5f);
    test.camera_body->Material ()->SetAngularDamping (0.5f);

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
