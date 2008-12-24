function test_scene ()
  print ("Scene test")

  local scene = Scene.Scene.Create ()

  print ("Name = " .. scene.Name)
  print ("Entities count = " .. scene.EntitiesCount)

  scene.Name = "scene1"

  local sound_emitter1 = Scene.SoundEmitter.Create ("sound.snddecl")

  sound_emitter1:BindToScene (scene)

  print ("Name = " .. scene.Name)
  print ("Entities count = " .. scene.EntitiesCount)

  sound_emitter1:BindToParent (scene.Root)

  print ("Entities count = " .. scene.EntitiesCount)
end

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
  
  node1:BindToParent (node3, Scene.NodeBindMode.AddRef, Scene.NodeTransformSpace.Parent)
  node2:BindToParent (node3)

  node4 = node3:FindChild ("node2")

  print ("node3 child name = " .. node4.Name)

  print_childs (node3)
    
  node1:Unbind (Scene.NodeTransformSpace.Parent)

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
  print ("LocalOrtX = " .. tostring (node1.LocalOrtX))
  print ("LocalOrtY = " .. tostring (node1.LocalOrtY))
  print ("LocalOrtZ = " .. tostring (node1.LocalOrtZ))
  print ("WorldOrtX = " .. tostring (node1.WorldOrtX))
  print ("WorldOrtY = " .. tostring (node1.WorldOrtY))
  print ("WorldOrtZ = " .. tostring (node1.WorldOrtZ))
  print ("ParentOrtX = " .. tostring (node1.ParentOrtX))
  print ("ParentOrtY = " .. tostring (node1.ParentOrtY))
  print ("ParentOrtZ = " .. tostring (node1.ParentOrtZ))

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

  local parent = Scene.Node.Create ()  
  local node2 = Scene.Node.Create ()

  node2:LookTo (vec3 (0, 2, 0), Scene.NodeOrt.Z, Scene.NodeOrt.X, Scene.NodeTransformSpace.World)

  print ("WorldOrtX = " .. tostring (node2.WorldOrtX))
  print ("WorldOrtY = " .. tostring (node2.WorldOrtY))
  print ("WorldOrtZ = " .. tostring (node2.WorldOrtZ))

  test_node_bind ()

  print ("IsInUpdateTransaction = " .. tostring (node1.IsInUpdateTransaction))

  node1:BeginUpdate ()

  print ("IsInUpdateTransaction = " .. tostring (node1.IsInUpdateTransaction))

  node1:EndUpdate ()

  print ("IsInUpdateTransaction = " .. tostring (node1.IsInUpdateTransaction))
end

function node_event_handler (node, event)
  local name = ""  
  
  if     event == Scene.NodeEvent.AfterUpdate       then name = "AfterUpdate"
  elseif event == Scene.NodeEvent.BeforeDestroy     then name = "BeforeDestroy"
  elseif event == Scene.NodeEvent.AfterDestroy      then name = "AfterDestroy"
  elseif event == Scene.NodeEvent.AfterBind         then name = "AfterBind"
  elseif event == Scene.NodeEvent.BeforeUnbind      then name = "BeforeUnbind"  
  elseif event == Scene.NodeEvent.AfterSceneAttach  then name = "AfterSceneAttach"
  elseif event == Scene.NodeEvent.BeforeSceneDetach then name = "BeforeSceneDetach"
  elseif event == Scene.NodeEvent.AfterSceneChange  then name = "AfterSceneChange"
  end  

  print ("Node '" .. tostring (node.Name) .. "' event '" .. name .. "'")
end

function node_subtree_event_handler (node, child, event)
  local name = ""  
  
  if     event == Scene.NodeSubTreeEvent.AfterBind    then name = "AfterBind"
  elseif event == Scene.NodeSubTreeEvent.BeforeUnbind then name = "BeforeUnbind"
  end  

  print ("Node '" .. tostring (node.Name) .. "." .. tostring (child.Name) .. "' subtree event '" .. name .. "'")
end

function test_events ()
  print ("Node events test")
  
  local handler1 = Scene.Node.CreateEventHandler ("node_event_handler")
  local handler2 = Scene.Node.CreateSubTreeEventHandler ("node_subtree_event_handler")
  local node1    = Scene.Node.Create ()
  local node2    = Scene.Node.Create ()
  local scene    = Scene.Scene.Create ()
  
  node1.Name = "Node1"
  node2.Name = "Node2"

  node1:RegisterEventHandler (Scene.NodeEvent.AfterUpdate, handler1)
  node1:RegisterEventHandler (Scene.NodeEvent.BeforeDestroy, handler1)
  node1:RegisterEventHandler (Scene.NodeEvent.AfterDestroy, handler1)
  node1:RegisterEventHandler (Scene.NodeEvent.AfterBind, handler1)
  node1:RegisterEventHandler (Scene.NodeEvent.BeforeUnbind, handler1)
  node1:RegisterEventHandler (Scene.NodeEvent.AfterSceneAttach, handler1)
  node1:RegisterEventHandler (Scene.NodeEvent.BeforeSceneDetach, handler1)
  node1:RegisterEventHandler (Scene.NodeEvent.AfterSceneChange, handler1)
  node2:RegisterSubTreeEventHandler (Scene.NodeSubTreeEvent.AfterBind, handler2)
  node2:RegisterSubTreeEventHandler (Scene.NodeSubTreeEvent.BeforeUnbind, handler2)

  node1:BindToScene (scene)
  node2:BindToScene (scene)
  node1:BindToParent (node2)
  
  node1.Position = vec3 (1, 2, 3)
  
  node1 = nil
  node2 = nil
  scene = nil
  
  collectgarbage ("collect")
end

function print_node_array (title, array)
  print (title .. " (array has " .. tostring (array.Size) .. " items):")

  for i=0,array.Size-1,1 do
    print ("  " .. array:Item (i).Name)
  end
end

function test_node_array (link_mode)
  if link_mode == Scene.NodeArrayLinkMode.AddRef  then print ("NodeArray addref test") end
  if link_mode == Scene.NodeArrayLinkMode.WeakRef then print ("NodeArray weak-ref test") end

  local node1 = Scene.Node.Create ()
  local node2 = Scene.Node.Create ()
  local node3 = Scene.Node.Create ()
  
  node1.Name = "node1"
  node2.Name = "node2"
  node3.Name = "node3"
  
  local array = Scene.NodeArray.Create (link_mode)
  local index = array:Add (node1)

  array:Add (node2)
  array:Add (node3)
  
  print_node_array ("after insert", array)
  
  node2 = nil
  
  collectgarbage ("collect")
  
  print_node_array ("after node2 destroy", array)
   
  array:Remove (index)
  
  print_node_array ("after remove node by index", array)

  array:Remove (node3)

  print_node_array ("after remove by value", array)
end

function test_entity ()
  print ("Entity test")

  local aabox1 = AABB ()
  local sound_emitter1 = Scene.SoundEmitter.Create ("sound.snddecl")

  print ("IsInfiniteBounds = " .. tostring (sound_emitter1.InfiniteBounds))
  print ("WireColor = " .. tostring (sound_emitter1.WireColor))

  sound_emitter1.WireColor = vec3 (1, 0, 0)

  print ("WireColor = " .. tostring (sound_emitter1.WireColor))

  sound_emitter1:SetWireColor (0, 0, 1)

  print ("WireColor = " .. tostring (sound_emitter1.WireColor))

  aabox1 = sound_emitter1.BoundBox

  print ("local bb minimum = " .. tostring (aabox1.Minimum))
  print ("local bb maximum = " .. tostring (aabox1.Maximum))

  aabox1 = sound_emitter1.WorldBoundBox

  print ("world bb minimum = " .. tostring (aabox1.Minimum))
  print ("world bb maximum = " .. tostring (aabox1.Maximum))

  aabox1 = sound_emitter1.ChildrenBoundBox

  print ("children bb minimum = " .. tostring (aabox1.Minimum))
  print ("children bb maximum = " .. tostring (aabox1.Maximum))

  aabox1 = sound_emitter1.FullBoundBox

  print ("full bb minimum = " .. tostring (aabox1.Minimum))
  print ("full bb maximum = " .. tostring (aabox1.Maximum))

  aabox1 = sound_emitter1.WorldChildrenBoundBox

  print ("world children bb minimum = " .. tostring (aabox1.Minimum))
  print ("world children bb maximum = " .. tostring (aabox1.Maximum))

  aabox1 = sound_emitter1.WorldFullBoundBox

  print ("world full bb minimum = " .. tostring (aabox1.Minimum))
  print ("world full bb maximum = " .. tostring (aabox1.Maximum))
end

function print_intersections (entity)
  local intersections = entity:GetIntersections ()
  
  print (entity.Name .. " has " .. tostring (intersections.Size) .. " intersections:")
  
  for i=0,intersections.Size-1,1 do
    print ("  " .. intersections:Item (i).Name)
  end
end

function test_intersections ()
  print ("Intersections test")
  
  local scene   = Scene.Scene.Create ()
  local entity1 = Scene.Helpers.Box.Create ()
  local entity2 = Scene.Helpers.Box.Create ()
  local entity3 = Scene.Helpers.Box.Create ()

  entity1.Name = "entity1"
  entity2.Name = "entity2"
  entity3.Name = "entity3"
  
  local bb = AABB (-1, -1, -1, 1, 1, 1)

  entity1.BoundBox = AABB (-1, -1, -1, 1, 1, 1)
  entity2.BoundBox = AABB (0.1, 0.1, -1, 1, 1, 1)
  entity3.BoundBox = AABB (-1, -1, -1, -0, -0.1, 1)  

  entity1:BindToScene (scene)
  entity2:BindToScene (scene)
  entity3:BindToScene (scene)

  print_intersections (entity1)
  print_intersections (entity2)
  print_intersections (entity3)
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

  local helper1 = Scene.Helpers.Box.Create ()
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
  sprite1.AlphaReference = 0.4

  print ("Alpha value = " .. sprite1.Alpha)
  print ("Alpha reference = " .. sprite1.AlphaReference)
  print ("Material name = " .. sprite1.Material)
  print ("Frame number = " .. sprite1.Frame)
   
  sprite1.Color    = vec4 (1, 0, 0, 0.7)

  print ("Color = " .. tostring (sprite1.Color))

  sprite1:SetColor (0, 0, 1, 0.3)

  print ("Color = " .. tostring (sprite1.Color))

  sprite1:SetColor (0, 1, 0)

  print ("Color = " .. tostring (sprite1.Color))
  
end

function test_sprite_pivot ()
  print ("Sprite pivot test")

  local sprite = Scene.Sprite.Create ()

  sprite.PivotPosition = vec3 (1, 2, 3)
  sprite.PivotRotation = 90

  local tm = sprite.WorldTMAfterPivot
 
  sprite:SetPosition (5, 6, 7)

  tm = sprite.WorldTMAfterPivot

  local position = tm * vec3 (0)
  local axis_x   = tm * vec4 (1, 0, 0, 0)
  local axis_y   = tm * vec4 (0, 1, 0, 0)
  local axis_z   = tm * vec4 (0, 0, 1, 0)

  print ("position: " .. tostring (position))
  print ("axis_x:   " .. tostring (axis_x))
  print ("axis_y:   " .. tostring (axis_y))
  print ("axis_z:   " .. tostring (axis_z))
end

function test_visual_model ()
  print ("VisualModel test")

  local visual_model1 = Scene.VisualModel.Create ()

  print ("Mesh name = " .. visual_model1.MeshName)

  visual_model1.MeshName = "mesh.xmesh"

  print ("Mesh name = " .. visual_model1.MeshName)
end

function get_name (alignment)
  if (alignment == Scene.TextLineAlignment.Center)   then return "Center"       end
  if (alignment == Scene.TextLineAlignment.Left)     then return "Left/Top"     end
  if (alignment == Scene.TextLineAlignment.Right)    then return "Right/Bottom" end
  if (alignment == Scene.TextLineAlignment.BaseLine) then return "BaseLine"     end
  return "Invalid"
end

function test_text_line ()
  print ("TextLine test")

  local text_line1 = Scene.TextLine.Create ()

  print ("Text = " .. text_line1.Text)
  print ("TextUnicode = " .. tostring (text_line1.TextUnicode))
  print ("Font = " .. text_line1.Font)
  print ("Color = " .. tostring (text_line1.Color))
  print ("Horizontal aligment = " .. get_name (text_line1.HorizontalAlignment) .. " vertical aligment = " .. get_name (text_line1.VerticalAlignment))

  text_line1.Text                = "text"
  text_line1.Font                = "font"
  text_line1.Color               = vec4 (0.1, 0.2, 0.3, 0.4)
  text_line1.HorizontalAlignment = Scene.TextLineAlignment.Center
  text_line1.VerticalAlignment   = Scene.TextLineAlignment.Bottom

  print ("Text = " .. text_line1.Text)
  print ("TextUnicode = " .. tostring (text_line1.TextUnicode))
  print ("Font = " .. text_line1.Font)
  print ("Color = " .. tostring (text_line1.Color))
  print ("Horizontal aligment = " .. get_name (text_line1.HorizontalAlignment) .. " vertical aligment = " .. get_name (text_line1.VerticalAlignment))

  text_line1.Text = "Non-unicode text"

  print ("Text = " .. text_line1.Text)
  print ("TextUnicode = " .. tostring (text_line1.TextUnicode))

  text_line1:SetColor (0.4, 0.3, 0.2, 0.1)

  print ("Color = " .. tostring (text_line1.Color))

  text_line1:SetColor (0.1, 0.2, 0.3)

  print ("Color = " .. tostring (text_line1.Color))

  text_line1:SetAlignment (Scene.TextLineAlignment.Right, Scene.TextLineAlignment.Center)

  print ("Horizontal aligment = " .. get_name (text_line1.HorizontalAlignment) .. " vertical aligment = " .. get_name (text_line1.VerticalAlignment))

  text_line1:SetAlignment (Scene.TextLineAlignment.BaseLine, Scene.TextLineAlignment.BaseLine)

  print ("Horizontal aligment = " .. get_name (text_line1.HorizontalAlignment) .. " vertical aligment = " .. get_name (text_line1.VerticalAlignment))
end

function test ()
  test_node ()
  
  test_events ()
  
  test_node_array (Scene.NodeArrayLinkMode.AddRef)
  test_node_array (Scene.NodeArrayLinkMode.WeakRef)

  test_entity ()
  
  test_intersections ()
  
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

  test_sprite_pivot ()

  test_scene ()

  test_text_line ()
end
