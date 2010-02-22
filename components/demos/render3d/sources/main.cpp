#include "shared.h"

const char*  SHADERS_DIR              = "data/shaders";
const char*  CONFIG_NAME              = "config.xml";
const char*  MODEL_NAME               = "data/meshes/main_ship.binmesh";
const char*  ENTERPRISE_MODEL_NAME    = "data/meshes/ship_01a.binmesh";
const char*  MATERIAL_LIBRARIES []    = {"data/materials/main_ship.xmtl", "data/materials/ship_01a.xmtl", "data/materials/sky.xmtl"};
const char*  SCENE_NAME               = "data/scenes/main_ship.xscene";
const char*  ENTERPRISE_SCENE_NAME    = "data/scenes/ship_01a.xscene";
const char*  REFLECTION_TEXTURE       = "data/textures/environment/EnvGala_000_D.tga";
const char*  SKY_MESH                 = "_SkyMesh";
const char*  SKY_MATERIAL             = "_SkyMaterial";
const int    ADDITIONAL_SHIPS_RANGE   = 70;
const size_t SKY_PARALLELS            = 30;
const size_t SKY_MERIDIANS            = 30;
const float  SKY_RADIUS               = 9000;
const float  PHYS_TARGET_REACH_DELAY  = 3.0f;

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
    for (SceneManager::NodesArray::iterator iter=test.scene_manager.Shattles ().begin (), end=test.scene_manager.Shattles ().end (); iter!=end; ++iter)
    {
      Node&                           enemy_node   = **iter;
      physics::low_level::IRigidBody& enemy_body   = *test.rigid_bodies [&enemy_node];
      Node*                           target_node  = 0;
      float                           min_distance = 0;            

      for (SceneManager::NodesArray::iterator iter=test.scene_manager.MainShips ().begin (), end=test.scene_manager.MainShips ().end (); iter!=end; ++iter)
      {
        Node& main_ship_node = **iter;
        float distance       = length (main_ship_node.WorldPosition () - enemy_node.WorldPosition ());
        
        if (!target_node || distance < min_distance)
        {
          distance    = min_distance;
          target_node = &main_ship_node;
        }
      }      
      
      if (!target_node)
        continue;                
        
      math::vec3f target_position     = target_node->WorldPosition (),
                  target_direction    = target_position - enemy_node.WorldPosition (),
                  current_velocity    = enemy_body.LinearVelocity (),
                  target_acceleration = 2.0f * (target_direction - current_velocity * PHYS_TARGET_REACH_DELAY) / PHYS_TARGET_REACH_DELAY / PHYS_TARGET_REACH_DELAY,
                  target_force        = target_acceleration * enemy_body.Mass ();                  
                  
      enemy_body.AddForce (target_force);
    }
    
//  test.camera_body->SetLinearVelocity (math::vec3f (test.x_camera_speed, 0, test.y_camera_speed) * inverse (test.camera->WorldTM ()));
//  test.camera_body->SetAngularVelocity (math::vec3f (test.y_camera_rotation_speed, test.x_camera_rotation_speed, test.z_camera_rotation_speed) * inverse (test.camera->WorldTM ()));
    test.camera_body->AddForce (math::vec3f (test.x_camera_speed, 0, test.y_camera_speed) * inverse (test.camera->WorldTM ()));
    test.camera_body->AddTorque (math::vec3f (test.y_camera_rotation_speed / 20.f, test.x_camera_rotation_speed / 20.f, test.z_camera_rotation_speed / 20.f) * inverse (test.camera->WorldTM ()));    
    
    test.physics_scene->PerformSimulation (dt);

    for (Test::RigidBodiesMap::iterator iter = test.rigid_bodies.begin (), end = test.rigid_bodies.end (); iter != end; ++iter)
    {
      const physics::low_level::Transform& body_transform = iter->second->WorldTransform ();

      iter->first->SetWorldPosition (body_transform.position);
      iter->first->SetWorldOrientation (body_transform.orientation);
    }
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

    for (size_t i = 0; i < 3; i++)
    {
      Node::Pointer enterprise = test.scene_manager.LoadScene (ENTERPRISE_SCENE_NAME);
      
      VertexArray verts;
      
      build_vertices (*enterprise, test, verts);

      enterprise->Translate ((float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2.f),
                             (float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2.f),
                             (float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2.f));                             

//      xtl::com_ptr<physics::low_level::IShape> shape = test.physics_driver->CreateSphereShape (8.f);

      xtl::com_ptr<physics::low_level::IShape> shape (test.physics_driver->CreateConvexShape (verts.size (), &verts [0]), false);

      RigidBodyPtr enterprise_body (test.physics_scene->CreateRigidBody (shape.get (), 1), false);

      physics::low_level::Transform enterprise_transform;

      enterprise_transform.position    = enterprise->WorldPosition ();
      enterprise_transform.orientation = enterprise->WorldOrientation ();      

      enterprise_body->SetWorldTransform (enterprise_transform);

      test.rigid_bodies.insert_pair (enterprise, enterprise_body);

      test.scene_manager.AddShattle (enterprise);
    }

    printf ("Load main ship scene\n");

    for (size_t i = 0; i < 1; i++)
    {
      Node::Pointer node = test.scene_manager.LoadScene (SCENE_NAME);
      
      VertexArray verts;
      
      build_vertices (*node, test, verts);
      
      node->Translate ((float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2),
                       (float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2),
                       (float)((int)(rand () % ADDITIONAL_SHIPS_RANGE) - ADDITIONAL_SHIPS_RANGE / 2));
                       
//      xtl::com_ptr<physics::low_level::IShape> shape = test.physics_driver->CreateSphereShape (3.f);
      xtl::com_ptr<physics::low_level::IShape> shape (test.physics_driver->CreateConvexShape (verts.size (), &verts [0]), false);

      RigidBodyPtr node_body (test.physics_scene->CreateRigidBody (shape.get (), 1), false);

      physics::low_level::Transform node_transform;

      node_transform.position    = node->WorldPosition ();
      node_transform.orientation = node->WorldOrientation ();

      node_body->SetWorldTransform (node_transform);

      test.rigid_bodies.insert_pair (node, node_body);

      test.scene_manager.AddMainShip (node);      
    }

    test.scene_manager.SetDrawMainShips (false);

    printf ("Register callbacks\n");

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));

    printf ("Main loop\n");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    for (;;);
  }

  return 0;
}
