function print_node (node)
  print (string.format ("Node position = [%.2f %.2f %.2f]", node.WorldPosition.x, node.WorldPosition.y, node.WorldPosition.z))
end

function print_bind (bind)
  print ("Bind anchor = " .. tostring (bind.Anchor) .. " axis = " .. tostring (bind.Axis))
end

function collision_filter (body1, body2)
  print ("Collision filter called")
  return true
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

function body_collision_callback (event, body1, body2, point)
  print ("Body collision callback called for event " .. collision_event_name (event) .. " at point " .. tostring (point))
end

function print_material (material)
  print (string.format ("Material linear damping = %.2f, angular damping = %.2f, friction = %.2f, anisotropic friction = %.2f %.2f %.2f, restitution = %.2f", material.LinearDamping, material.AngularDamping, material.Friction,
                        material.AnisotropicFriction.x, material.AnisotropicFriction.y, material.AnisotropicFriction.z, material.Restitution))
end

function print_shape_list_shape_info (list, index)
  print (string.format ("Shape list %d shape margin = %.2f position is %s orientation is %s", index, list:Shape (index).Margin, tostring (list:ShapePosition (index)), tostring (list:ShapeOrientation (index))))
end

function body_update_transform_callback (body)
  print ("body transform updated")
end

function print_rigid_body (body)
  print ("Rigid body:")
  print (string.format ("  mass %.2f", body.Mass))
  print ("  mass space inertia tensor " .. tostring (body.MassSpaceInertiaTensor))
  print ("  world position " .. tostring (body.WorldPosition))
  print ("  world orientation " .. tostring (body.WorldOrientation))
  print (string.format ("  sleep linear velocity %.2f", body.SleepLinearVelocity))
  print (string.format ("  sleep angular velocity %.2f", body.SleepAngularVelocity))
  print (string.format ("  ccd motion threshold %.2f", body.CcdMotionThreshold))
  print (string.format ("  shape margin %.2f", body.Shape.Margin))
  print (string.format ("  material linear damping %.2f", body.Material.LinearDamping))
  print ("  collision group '" .. body.CollisionGroup .. "'")
  print ("  linear velocity " .. tostring (body.LinearVelocity))
  print ("  angular velocity " .. tostring (body.AngularVelocity))
  print ("  flags:")
  print ("    frozen position x = " .. tostring (body:GetFlag (Physics.RigidBodyFlag.FrozenPositionX)))
  print ("    frozen position y = " .. tostring (body:GetFlag (Physics.RigidBodyFlag.FrozenPositionY)))
  print ("    frozen position z = " .. tostring (body:GetFlag (Physics.RigidBodyFlag.FrozenPositionZ)))
  print ("    frozen position = " .. tostring (body:GetFlag (Physics.RigidBodyFlag.FrozenPosition)))
  print ("    frozen rotation x = " .. tostring (body:GetFlag (Physics.RigidBodyFlag.FrozenRotationX)))
  print ("    frozen rotation y = " .. tostring (body:GetFlag (Physics.RigidBodyFlag.FrozenRotationY)))
  print ("    frozen rotation z = " .. tostring (body:GetFlag (Physics.RigidBodyFlag.FrozenRotationZ)))
  print ("    frozen rotation = " .. tostring (body:GetFlag (Physics.RigidBodyFlag.FrozenRotation)))
  print ("    kinematic = " .. tostring (body:GetFlag (Physics.RigidBodyFlag.Kinematic)))
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
  local plane_shape   = manager:CreatePlaneShape (vec3 (0, 1, 0), 10)
  
  local shape_list = Physics.ShapeList.Create ()
  
  print ("Shape list size = " .. shape_list.Size)
  
  shape_list:Add (box_shape)
  shape_list:Add (sphere_shape, vec3 (1, 2, 3))
  shape_list:Add (capsule_shape, quat (1, 2, 3, 4))
  shape_list:Add (plane_shape, vec3 (5, 6, 7), quat (1, 2, 3, 4))

  local compound_shape = manager:CreateCompoundShape (shape_list)
  
  print ("Shape list size = " .. shape_list.Size)

  print_shape_list_shape_info (shape_list, 0)
  print_shape_list_shape_info (shape_list, 1)
  print_shape_list_shape_info (shape_list, 2)
  print_shape_list_shape_info (shape_list, 3)

  shape_list:Remove (1)
  
  print ("Shape list size = " .. shape_list.Size)
  
  shape_list:Clear ()
  
  print ("Shape list size = " .. shape_list.Size)
  
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
  local body2 = scene:CreateRigidBody (plane_shape, 2)
  
  print_rigid_body (body1)
  print_rigid_body (body2)

  body1.Mass = 4
  body1.MassSpaceInertiaTensor = vec3 (1, 2, 3)
  body1.WorldPosition = vec3 (2, 3, 4)
  body1.WorldOrientation = quat (1, 2, 3, 4)
  body1.SleepLinearVelocity = 5
  body1.SleepAngularVelocity = 6
  body1.CcdMotionThreshold = 7
  body1.CollisionGroup = "object"
  body1.LinearVelocity = vec3 (7, 8, 9)
  body1.AngularVelocity = vec3 (8, 9, 10)
  body1.Material = material  
  body1:SetFlag (Physics.RigidBodyFlag.FrozenPositionX, true)  
  body1:SetFlag (Physics.RigidBodyFlag.FrozenRotationY, true)  
  body1:SetFlag (Physics.RigidBodyFlag.Kinematic, true)  
  
  body2.CollisionGroup = "ground"
  
  print_rigid_body (body1)
  
  body1:SetFlag (Physics.RigidBodyFlag.FrozenPositionX, true)  
  body1:SetFlag (Physics.RigidBodyFlag.FrozenPositionY, true)  
  body1:SetFlag (Physics.RigidBodyFlag.FrozenPositionZ, true)  
  body1:SetFlag (Physics.RigidBodyFlag.FrozenRotation, true)  
  body1:SetFlag (Physics.RigidBodyFlag.FrozenRotationY, false)  
  
  print_rigid_body (body1)

  body1:SetFlag (Physics.RigidBodyFlag.FrozenPosition, false)  
  body1:SetFlag (Physics.RigidBodyFlag.FrozenRotation, false)  
  body1:SetFlag (Physics.RigidBodyFlag.Kinematic, false)  

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
  
  local callback_connection       = scene:RegisterCollisionCallback ("*", "*", Physics.CollisionEventType.Begin, Physics.Scene.CreateCollisionCallback (collision_callback))
  local body_transform_connection = body1:RegisterTransformUpdateCallback (Physics.RigidBody.CreateTransformUpdateCallback (body_update_transform_callback))
  local body_collision_connection = body1:RegisterCollisionCallback ("*", Physics.CollisionEventType.Begin, Physics.RigidBody.CreateCollisionCallback (body_collision_callback))

  local node = Scene.Node.Create ()
  
  print_node (node)
  
  scene:PerformSimulation (1)

  local node_controller = Scene.Controllers.SyncPhysicsToNode.Create (node, body1)

  print_node (node)

  node:Update (0.1)

  print_node (node)
end

function test ()
  test_physics ()
end
