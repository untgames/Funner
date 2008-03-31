function test_node_bind ()
  print ("Node bind test")

  node1 = Scene.Node.Create ()
  node2 = Scene.Node.Create ()
  node3 = Scene.Node.Create ()

  node4 = node2:Parent ()

  node1:BindToParent (node3, Scene_NodeBindMode.AddRef, Scene_NodeTransformSpace.Parent)
  node2:BindToParent (node3)
  node1:Unbind (Scene_NodeTransformSpace.Parent)
  node3:UnbindAllChildren ()
end

function test_node ()
  print ("Node test")

  node1 = Scene.Node.Create ()

  print ("Name = " .. node1.Name)
  print ("Position = " .. tostring (node1.Position))
  print ("WorldPosition = " .. tostring (node1.WorldPosition))
  print ("Orientation = " .. tostring (node1.Orientation))
  print ("WorldOrientation = " .. tostring (node1.WorldOrientation))
  print ("Scale = " .. tostring (node1.Scale))
  print ("WorldScale = " .. tostring (node1.WorldScale))
  print ("OrientationInherit = " .. tostring (node1.OrientationInherit))
  print ("ScaleInherit = " .. tostring (node1.ScaleInherit))
  print ("LocalTM = " .. tostring (node1.LocalTM))
  print ("WorldTM = " .. tostring (node1.WorldTM))
  print ("ParentTM = " .. tostring (node1.ParentTM))

  node1.Name = "node1"
  node1.Position = vec3 (10, 10, 10)
  node1.Orientation = quat (34, 10, 15, 0)
  node1.Scale = vec3 (100, 0, 0)
  node1.OrientationInherit = false
  node1.ScaleInherit = false

  print ("Name = " .. node1.Name)
  print ("Position = " .. tostring (node1.Position))
  print ("WorldPosition = " .. tostring (node1.WorldPosition))
  print ("Orientation = " .. tostring (node1.Orientation))
  print ("WorldOrientation = " .. tostring (node1.WorldOrientation))
  print ("Scale = " .. tostring (node1.Scale))
  print ("WorldScale = " .. tostring (node1.WorldScale))
  print ("OrientationInherit = " .. tostring (node1.OrientationInherit))
  print ("ScaleInherit = " .. tostring (node1.ScaleInherit))
  print ("LocalTM = " .. tostring (node1.LocalTM))
  print ("WorldTM = " .. tostring (node1.WorldTM))
  print ("ParentTM = " .. tostring (node1.ParentTM))

  node1:SetPosition (1.4, 5, 6.779)
  print ("Position = " .. tostring (node1.Position))

  node1:ResetPosition ()
  print ("Position = " .. tostring (node1.Position))

  node1:SetOrientation (41, 15.4, 48, 34)
  print ("Orientation = " .. tostring (node1.Orientation))

  node1:SetEulerOrientation (49, 1.4, 27)
  print ("Orientation = " .. tostring (node1.Orientation))

  node1:ResetOrientation ()
  print ("Orientation = " .. tostring (node1.Orientation))

  node1:SetScale (15, 16.4, 17)
  print ("Scale = " .. tostring (node1.Scale))

  node1:Rescale (vec3 (10, 1, 1))
  print ("Scale = " .. tostring (node1.Scale))

  print ("ObjectTM = " .. tostring (node1:ObjectTM (node1)))

  node1:ResetScale ()
  print ("Scale = " .. tostring (node1.Scale))

  test_node_bind ()
end

function test_perspective_camera ()
  print ("Perspective camera test")

  camera1 = Scene.PerspectiveCamera.Create ()

  print ("FovX  = " .. camera1.FovX)
  print ("FovY  = " .. camera1.FovY)
  print ("ZNear = " .. camera1.ZNear)
  print ("ZFar  = " .. camera1.ZFar)
  camera1.FovX  = 36
  camera1.FovY  = 20
  camera1.ZNear = 10
  camera1.ZFar  = 1000
  print ("FovX = " .. camera1.FovX)
  print ("FovY  = " .. camera1.FovY)
  print ("ZNear = " .. camera1.ZNear)
  print ("ZFar  = " .. camera1.ZFar)
end

function test_ortho_camera ()
  print ("Ortho camera test")

  camera1 = Scene.OrthoCamera.Create ()

  print ("Left   = " .. camera1.Left)
  print ("Right  = " .. camera1.Right)
  print ("Top    = " .. camera1.Top)
  print ("Bottom = " .. camera1.Bottom)
  print ("ZNear  = " .. camera1.ZNear)
  print ("ZFar   = " .. camera1.ZFar)
  camera1.Left   = -18
  camera1.Right  = 18
  camera1.Top    = 10
  camera1.Bottom = -10
  camera1.ZNear  = 10
  camera1.ZFar   = 1000
  print ("Left   = " .. camera1.Left)
  print ("Right  = " .. camera1.Right)
  print ("Top    = " .. camera1.Top)
  print ("Bottom = " .. camera1.Bottom)
  print ("ZNear  = " .. camera1.ZNear)
  print ("ZFar   = " .. camera1.ZFar)
end

function test_light ()
  print ("Common light test (point light)")

  light1 = Scene.PointLight.Create ()

  print ("LightColor = " .. tostring (light1.LightColor))
  print ("Attenuation = " .. tostring (light1.Attenuation))
  print ("Range = " .. light1.Range)

  light1.LightColor  = vec3 (1, 0, 0)
  light1.Attenuation = vec3 (1, 1, 0)
  light1.Range       = 579

  print ("LightColor = " .. tostring (light1.LightColor))
  print ("Attenuation = " .. tostring (light1.Attenuation))
  print ("Range = " .. light1.Range)
end

function test_direct_light ()
  print ("Direct light test")

  light1 = Scene.DirectLight.Create ()

  print ("Radius = " .. light1.Radius)

  light1.Radius = 763

  print ("Radius = " .. light1.Radius)
end

function test_spot_light ()
  print ("Spot light test")

  light1 = Scene.SpotLight.Create ()

  print ("Name = " .. light1.Name)
  print ("Angle = " .. light1.Angle)
  print ("Exponent = " .. light1.Exponent)

  light1.Angle = 75
  light1.Exponent = 0.7
  light1.Name = "light1"

  print ("Name = " .. light1.Name)
  print ("Angle = " .. light1.Angle)
  print ("Exponent = " .. light1.Exponent)
end

function test_box_helper ()
  print ("BoxHelper test")

  helper1 = Scene.BoxHelper.Create ()
end

function test_listener ()
  print ("Listener test")

  listener1 = Scene.Listener.Create ()

  print ("Gain = " .. listener1.Gain)

  listener1.Gain = 0.75

  print ("Gain = " .. listener1.Gain)
end

function test_sound_emitter ()
  print ("SoundEmitter test")

  sound_emitter1 = Scene.SoundEmitter.Create ("sound.snddecl")

  sound_emitter1:Play ()
  sound_emitter1:Stop ()
end

function test_visual_model ()
  print ("VisualModel test")

  visual_model1 = Scene.VisualModel.Create ()

  print ("Mesh name = " .. visual_model1.MeshName)

  visual_model1.MeshName = "mesh.xmesh"

  print ("Mesh name = " .. visual_model1.MeshName)
end

function test ()
  test_node ()
  
  test_perspective_camera ()
  test_ortho_camera ()

  test_light ()
  test_direct_light ()
  test_spot_light ()

  test_box_helper ()

  test_listener ()

  test_sound_emitter ()

  test_visual_model ()
end
