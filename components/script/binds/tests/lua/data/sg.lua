function print_childs (parent_node)
  local child_node = parent_node:FirstChild ()
  while (child_node) do
    print ("Child name - " .. child_node.Name) 
    child_node = child_node:NextChild ()
  end
end

function test_node_bind ()
  print ("Node bind test")

  local node1 = Scene.Node.Create ()
  local node2 = Scene.Node.Create ()
  local node3 = Scene.Node.Create ()

  node1.Name = "node1"
  node2.Name = "node2"
  node3.Name = "node3"

  local node4 = node2:Parent ()
  
  node1:BindToParent (node3, Scene_NodeBindMode.AddRef, Scene_NodeTransformSpace.Parent)
  node2:BindToParent (node3)

  node4 = node3:FindChild ("node2")

  print ("node3 child name = " .. node4.Name)

  print_childs (node3)
    
  node1:Unbind (Scene_NodeTransformSpace.Parent)

  print ("Unbind node1 test")
  print_childs (node3)
    
  node3:UnbindAllChildren ()

  print ("Unbind all test")
  print_childs (node3)
    
  node1:BindToParent (node3)
  node2:BindToParent (node3)

  print ("Unbind node2 by name test")

  node3:UnbindChild ("node2")

  print_childs (node3)
end

function test_node ()
  print ("Node test")

  local node1 = Scene.Node.Create ()

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
  print ("Reset position = " .. tostring (node1.Position))

  node1:Translate (4, 5, 6)
  print ("Translate position = " .. tostring (node1.Position))

  node1:SetOrientation (41, 15.4, 48, 34)
  print ("Orientation = " .. tostring (node1.Orientation))

  node1:SetEulerOrientation (49, 1.4, 27)
  print ("Orientation = " .. tostring (node1.Orientation))

  node1:ResetOrientation ()
  print ("Reset orientation = " .. tostring (node1.Orientation))

  node1:Rotate (15, 30, 45)
  print ("Roatate orientation = " .. tostring (node1.Orientation))

  node1:SetScale (15, 16.4, 17)
  print ("Scale = " .. tostring (node1.Scale))

  node1:Rescale (vec3 (10, 1, 1))
  print ("Rescale = " .. tostring (node1.Scale))

  print ("ObjectTM = " .. tostring (node1:ObjectTM (node1)))

  node1:ResetScale ()
  print ("Scale = " .. tostring (node1.Scale))

  test_node_bind ()
end

function test_entity ()
  print ("Entity test")

  local aabox1 = BV.AxisAlignedBox.create ()
  local sound_emitter1 = Scene.SoundEmitter.Create ("sound.snddecl")

  print ("IsInfiniteBounds = " .. tostring (sound_emitter1:IsInfiniteBounds ()))
  print ("WireColor = " .. tostring (sound_emitter1.WireColor))

  sound_emitter1.WireColor = vec3 (1, 0, 0)

  print ("WireColor = " .. tostring (sound_emitter1.WireColor))

  sound_emitter1:SetWireColor (0, 0, 1)

  print ("WireColor = " .. tostring (sound_emitter1.WireColor))

  aabox1 = sound_emitter1:BoundBox ()

  print ("local bb minimum = " .. tostring (aabox1.minimum))
  print ("local bb maximum = " .. tostring (aabox1.maximum))

  aabox1 = sound_emitter1:WorldBoundBox ()

  print ("world bb minimum = " .. tostring (aabox1.minimum))
  print ("world bb maximum = " .. tostring (aabox1.maximum))

  aabox1 = sound_emitter1:ChildrenBoundBox ()

  print ("children bb minimum = " .. tostring (aabox1.minimum))
  print ("children bb maximum = " .. tostring (aabox1.maximum))

  aabox1 = sound_emitter1:FullBoundBox ()

  print ("full bb minimum = " .. tostring (aabox1.minimum))
  print ("full bb maximum = " .. tostring (aabox1.maximum))

  aabox1 = sound_emitter1:WorldChildrenBoundBox ()

  print ("world children bb minimum = " .. tostring (aabox1.minimum))
  print ("world children bb maximum = " .. tostring (aabox1.maximum))

  aabox1 = sound_emitter1:WorldFullBoundBox ()

  print ("world full bb minimum = " .. tostring (aabox1.minimum))
  print ("world full bb maximum = " .. tostring (aabox1.maximum))
end

function test_perspective_camera ()
  print ("Perspective camera test")

  local camera1 = Scene.PerspectiveCamera.Create ()

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

  local camera1 = Scene.OrthoCamera.Create ()

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

  local light1 = Scene.PointLight.Create ()

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

  local light1 = Scene.DirectLight.Create ()

  print ("Radius = " .. light1.Radius)

  light1.Radius = 763

  print ("Radius = " .. light1.Radius)
end

function test_spot_light ()
  print ("Spot light test")

  local light1 = Scene.SpotLight.Create ()

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

  local helper1 = Scene.BoxHelper.Create ()
end

function test_listener ()
  print ("Listener test")

  local listener1 = Scene.Listener.Create ()

  print ("Gain = " .. listener1.Gain)

  listener1.Gain = 0.75

  print ("Gain = " .. listener1.Gain)
end

function test_sound_emitter ()
  print ("SoundEmitter test")

  local sound_emitter1 = Scene.SoundEmitter.Create ("sound.snddecl")

  print ("Gain = " .. sound_emitter1.Gain)

  sound_emitter1.Gain = 0.7
  
  print ("Gain = " .. sound_emitter1.Gain)

  sound_emitter1:Play ()
  sound_emitter1:Stop ()

  print ("SoundEmitter sound declaration name = " .. sound_emitter1.SoundDeclarationName)
end

function test_sprite ()
  print ("Sprite test")

  local sprite1 = Scene.Sprite.Create ()

  print ("Color = " .. tostring (sprite1.Color))
  print ("Material name = " .. sprite1.Material)

  sprite1.Material = "SettedMaterial"
  sprite1.Alpha    = 0.8
  sprite1.Frame    = 9

  print ("Alpha value = " .. sprite1.Alpha)
  print ("Material name = " .. sprite1.Material)
  print ("Frame number = " .. sprite1.Frame)
   
  sprite1.Color    = vec4 (1, 0, 0, 0.7)

  print ("Color = " .. tostring (sprite1.Color))

  sprite1:SetColor (0, 0, 1, 0.3)

  print ("Color = " .. tostring (sprite1.Color))

  sprite1:SetColor (0, 1, 0)

  print ("Color = " .. tostring (sprite1.Color))
  
end

function test_visual_model ()
  print ("VisualModel test")

  local visual_model1 = Scene.VisualModel.Create ()

  print ("Mesh name = " .. visual_model1.MeshName)

  visual_model1.MeshName = "mesh.xmesh"

  print ("Mesh name = " .. visual_model1.MeshName)
end

function test ()
  test_node ()

  test_entity ()
  
  test_perspective_camera ()
  test_ortho_camera ()

  test_light ()
  test_direct_light ()
  test_spot_light ()

  test_box_helper ()

  test_listener ()

  test_sound_emitter ()

  test_visual_model ()

  test_sprite ()
end
