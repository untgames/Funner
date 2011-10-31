function print_bind (bind)
  print ("Bind anchor = " .. tostring (bind.Anchor) .. " axis = " .. tostring (bind.Axis))
end

function collision_filter (body1, body2)
  print ("Collision filter called")
end

function collision_event_name (event)
  if event == Physics.CollisionEventType.Begin then
    return "Begin"
  elseif event == Physics.CollisionEventType.Process then
    return "Process"
  elseif event == Physics.CollisionEventType.End then
    return "End"
  else
    return "Unknown"
  end
end

function collision_callback (event, body1, body2, point)
  print ("Collision callback called for event " .. collision_event_name (event) .. " at point " .. tostring (point))
end

function print_material (material)
  print (string.format ("Material linear damping = %.2f, angular damping = %.2f, friction = %.2f, anisotropic friction = %.2f %.2f %.2f, restitution = %.2f", material.LinearDamping, material.AngularDamping, material.Friction,
                        material.AnisotropicFriction.x, material.AnisotropicFriction.y, material.AnisotropicFriction.z, material.Restitution))
end

function test_physics()
  print "Physics test"

  local manager = Engine.PhysicsManagers.Get ("PhysicsManager")
  
  print ("Manager description is '" .. manager.Description .. "'")
  
  local scene         = manager:CreateScene ()
  local material      = manager:CreateMaterial ()
  local box_shape     = manager:CreateBoxShape (vec3 (1, 2, 3))
  local sphere_shape  = manager:CreateSphereShape (1)
  local capsule_shape = manager:CreateCapsuleShape (1, 2)
  local plane_shape   = manager:CreatePlaneShape (vec3 (1, 2, 3), 4)
  
  print_material (material)

  material.LinearDamping       = 1
  material.AngularDamping      = 2
  material.Friction            = 3
  material.AnisotropicFriction = vec3 (4, 5, 6)
  material.Restitution         = 7

  print_material (material)
  
  print (string.format ("box_shape margin = %.2f", box_shape.Margin))
  
  box_shape.Margin = 5
  
  print ("new box_shape margin = " .. box_shape.Margin)
  
  print ("Scene gravity = " .. tostring (scene.Gravity))
  print (string.format ("Scene simulation step = %0.2f", scene.SimulationStep))
  
  scene.Gravity = vec3 (0, -8, 0)
  scene.SimulationStep = 0.1
  
  print ("Scene gravity = " .. tostring (scene.Gravity))
  print (string.format ("Scene simulation step = %0.2f", scene.SimulationStep))
  
  local body1 = scene:CreateRigidBody (box_shape, 1)
  local body2 = scene:CreateRigidBody (box_shape, 2)
  
  local joint_bind_1 = Physics.JointBind.Create (body1)
  local joint_bind_2 = Physics.JointBind.Create (body1, vec3 (1, 2, 3))
  local joint_bind_3 = Physics.JointBind.Create (body2, vec3 (2, 3, 4), vec3 (1, 0, 0))
  
  print_bind (joint_bind_1)
  print_bind (joint_bind_2)
  print_bind (joint_bind_3)
  
  joint_bind_1.Body = body2
  joint_bind_1.Anchor = vec3 (3, 4, 5)
  joint_bind_1.Axis = vec3 (0, 1, 0)
  
  print_bind (joint_bind_1)
  
  local spherical_joint    = scene:CreateSphericalJoint (joint_bind_1, joint_bind_2)
  local cone_twist_joint_1 = scene:CreateConeTwistJoint (joint_bind_1, joint_bind_2)
  local cone_twist_joint_2 = scene:CreateConeTwistJoint (joint_bind_1, joint_bind_2, 10)
  local cone_twist_joint_3 = scene:CreateConeTwistJoint (joint_bind_1, joint_bind_2, 20, 30)
  local hinge_joint        = scene:CreateHingeJoint     (joint_bind_1, joint_bind_2)
  local prismatic_joint    = scene:CreatePrismaticJoint (joint_bind_1, joint_bind_2)

  print ("hinge joint bodies count = " .. hinge_joint.BodiesCount)  

  local filter1 = scene:AddCollisionFilter ("*", "*", true)
  local filter2 = scene:AddCollisionFilter ("*", "ground", false, Physics.Scene.CreateCollisionFilter (collision_filter))
  
  scene:RemoveCollisionFilter (filter1)
  scene:RemoveAllCollisionFilters ()

  local filter3 = scene:AddCollisionFilter ("*", "*", true, Physics.Scene.CreateCollisionFilter (collision_filter))
  
  local callback_connection = scene:RegisterCollisionCallback ("*", "*", Physics.CollisionEventType.Begin, Physics.Scene.CreateCollisionCallback (collision_callback))
end

function test ()
  test_physics ()
end
