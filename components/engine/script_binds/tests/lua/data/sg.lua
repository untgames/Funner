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
  local child_node = parent_node.FirstChild
  while (child_node) do
    print ("Child name - " .. child_node.Name) 
    child_node = child_node.NextChild
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

  local node4 = node2.Parent
  
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

function test_node_pivot ()
  print ("Node pivot test")
  
  local node = Scene.Node.Create ()
  local parent = Scene.Node.Create ()
  
  parent:SetEulerOrientation (0, 0, 90)
  parent:SetPivotPosition (1, 0, 0)
  node:SetPosition (2, 0, 0)
  node:SetEulerOrientation (0, 0, 90)
  
  node:BindToParent (parent)
  
  print (string.format ("parent position: local=[%s] world=[%s]", tostring (parent.Position), tostring (parent.WorldPosition)))
  print (string.format ("node position: local=[%s] world=[%s]", tostring (node.Position), tostring (node.WorldPosition)))
  
  local test = node.WorldTM * vec3 (2, 0, 0)
  
  print (string.format ("test point: [%s]", tostring (test)))
  
  local saved_position = node.Position
  
  node:Translate (0, -2, 0, Scene.NodeTransformSpace.Local)
  
  print (string.format ("test local translate: local=[%s] world=[%s]", tostring (node.Position), tostring (node.WorldPosition)))
  
  node.Position = saved_position
  
  node:Translate (0, -2, 0, Scene.NodeTransformSpace.Parent)
  
  print (string.format ("test parent translate: local=[%s] world=[%s]", tostring (node.Position), tostring (node.WorldPosition)))
  
  node.Position = saved_position

  node:Translate (0, -2, 0, Scene.NodeTransformSpace.World)
  
  print (string.format ("test world translate: local=[%s] world=[%s]", tostring (node.Position), tostring (node.WorldPosition)))
  
  print ("create sub child")
  
  local child = Scene.Node.Create ()
  
  child:BindToParent (node)
 
  node:SetPosition (2, 0, 0)
  node:SetScale (2, 2, 1)
  
  node.PivotPosition = vec3 (1, 0, 0)
  
  child:SetPosition    (-1, -1, 0)
  child:SetPivotPosition (-1, 0, 0)
  child:SetEulerOrientation (0, 0, 90)

  print (string.format ("test node position: local=[%s] world=[%s]", tostring (node.Position), tostring (node.WorldPosition)))
  print (string.format ("test child position: local=[%s] world=[%s]", tostring (child.Position), tostring (child.WorldPosition)))
  
  test = child.WorldTM * vec3 (1, 1, 0)
  
  print (string.format ("test point: [%s]", tostring (test)))
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
  collectgarbage ("collect")
end

function test_node_properties ()
  local node       = Scene.Node.Create ()
  local properties = Common.PropertyMap.Create ()
  
  node.Properties = properties

  node.Properties:SetString ("String", "hello")  
  node.Properties:SetInteger ("Integer", 1)
  node.Properties:SetFloat ("Float", 3.14)
  node.Properties:SetVector ("Vector", vec4 (1, 2, 3, 4))
  node.Properties:SetMatrix ("Matrix", mat4 (3))
  
  local string_result = node.Properties:GetString ("String")
  local int_result    = node.Properties:GetInteger ("Integer")
  local float_result  = node.Properties:GetFloat ("Float")
  local vec_result    = node.Properties:GetVector ("Vector")
  local matrix_result = node.Properties:GetMatrix ("Matrix")

  print (string.format ("result: string='%s' int=%d float=%.3f vec=[%.3f %.3f %.3f %.3f] det(matrix)=%.3f",
    string_result, int_result, float_result, vec_result.x, vec_result.y, vec_result.z, vec_result.w, matrix_result:det ()))

  node.Properties:RemoveProperty ("Float")

  print ("is_present(Float)=" .. tostring (node.Properties:IsPresent ("Float")))
  print ("is_present(String)=" .. tostring (node.Properties:IsPresent ("String")))
  
  print (string.format ("node has %u properties", node.Properties.Size))
  
  for i=0,node.Properties.Size-1 do
    print (string.format ("  #%d: name='%s' type=%d", i, node.Properties:GetPropertyName (i), node.Properties:GetPropertyType (i)))
  end
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

  print ("FovX  = " .. tostring (camera1.FovX))
  print ("FovY  = " .. tostring (camera1.FovY))
  print ("ZNear = " .. tostring (camera1.ZNear))
  print ("ZFar  = " .. tostring (camera1.ZFar))
  camera1.FovX  = 36
  camera1.FovY  = 20
  camera1.ZNear = 10
  camera1.ZFar  = 1000
  print ("FovX = " .. tostring (camera1.FovX))
  print ("FovY  = " .. tostring (camera1.FovY))
  print ("ZNear = " .. tostring (camera1.ZNear))
  print ("ZFar  = " .. tostring (camera1.ZFar))
end

function test_ortho_camera ()
  print ("Ortho camera test")

  local camera1 = Scene.OrthoCamera.Create ()

  print ("Left   = " .. tostring (camera1.Left))
  print ("Right  = " .. tostring (camera1.Right))
  print ("Top    = " .. tostring (camera1.Top))
  print ("Bottom = " .. tostring (camera1.Bottom))
  print ("ZNear  = " .. tostring (camera1.ZNear))
  print ("ZFar   = " .. tostring (camera1.ZFar))
  camera1.Left   = -18
  camera1.Right  = 18
  camera1.Top    = 10
  camera1.Bottom = -10
  camera1.ZNear  = 10
  camera1.ZFar   = 1000
  print ("Left   = " .. tostring (camera1.Left))
  print ("Right  = " .. tostring (camera1.Right))
  print ("Top    = " .. tostring (camera1.Top))
  print ("Bottom = " .. tostring (camera1.Bottom))
  print ("ZNear  = " .. tostring (camera1.ZNear))
  print ("ZFar   = " .. tostring (camera1.ZFar))
end

function test_light ()
  print ("Common light test (point light)")

  local light1 = Scene.PointLight.Create ()

  print ("LightColor = " .. tostring (light1.LightColor))
  print ("Attenuation = " .. tostring (light1.Attenuation))
  print ("Range = " .. tostring (light1.Range))

  light1.LightColor  = vec3 (1, 0, 0)
  light1.Attenuation = vec3 (1, 1, 0)
  light1.Range       = 579

  print ("LightColor = " .. tostring (light1.LightColor))
  print ("Attenuation = " .. tostring (light1.Attenuation))
  print ("Range = " .. tostring (light1.Range))
end

function test_direct_light ()
  print ("Direct light test")

  local light1 = Scene.DirectLight.Create ()

  print ("Radius = " .. tostring (light1.Radius))

  light1.Radius = 763

  print ("Radius = " .. tostring (light1.Radius))
end

function test_spot_light ()
  print ("Spot light test")

  local light1 = Scene.SpotLight.Create ()

  print ("Name = " .. light1.Name)
  print ("Angle = " .. tostring (light1.Angle))
  print ("Exponent = " .. tostring (light1.Exponent))

  light1.Angle = 75
  light1.Exponent = 0.7
  light1.Name = "light1"

  print ("Name = " .. light1.Name)
  print ("Angle = " .. tostring (light1.Angle))
  print ("Exponent = " .. tostring (light1.Exponent))
end

function test_box_helper ()
  print ("BoxHelper test")

  local helper1 = Scene.Helpers.Box.Create ()
end

function test_listener ()
  print ("Listener test")

  local listener1 = Scene.Listener.Create ()

  print ("Gain = " .. tostring (listener1.Gain))

  listener1.Gain = 0.75

  print ("Gain = " .. tostring (listener1.Gain))
end

function test_sound_emitter ()
  print ("SoundEmitter test")

  local sound_emitter1 = Scene.SoundEmitter.Create ("sound.snddecl")

  print ("Gain = " .. tostring (sound_emitter1.Gain))
  print ("PlayStartOffset = " .. tostring (sound_emitter1.PlayStartOffset))

  sound_emitter1.Gain = 0.7
  
  print ("Gain = " .. tostring (sound_emitter1.Gain))

  sound_emitter1:Play ()
  sound_emitter1:Stop ()

  print ("SoundEmitter sound declaration name = " .. sound_emitter1.SoundDeclarationName)
  print ("PlayStartOffset = " .. tostring (sound_emitter1.PlayStartOffset))
  
  sound_emitter1:Play (1)

  print ("PlayStartOffset = " .. tostring (sound_emitter1.PlayStartOffset))
end

function test_sprite ()
  print ("Sprite test")

  local sprite1 = Scene.Sprite.Create ()

  print ("Color = " .. tostring (sprite1.Color))
  print ("Material name = " .. sprite1.Material)

  sprite1.Material = "SettedMaterial"
  sprite1.Alpha    = 0.8

  print ("Alpha value = " .. tostring (sprite1.Alpha))
  print ("Material name = " .. sprite1.Material)
   
  sprite1.Color    = vec4 (1, 0, 0, 0.7)

  print ("Color = " .. tostring (sprite1.Color))

  sprite1:SetColor (0, 0, 1, 0.3)

  print ("Color = " .. tostring (sprite1.Color))

  sprite1:SetColor (0, 1, 0)

  print ("Color = " .. tostring (sprite1.Color))
  
end

function test_line ()
  print ("Line test")

  local line1 = Scene.Line.Create ()

  print ("Color0 = " .. tostring (line1.Color0))
  print ("Color1 = " .. tostring (line1.Color1))
  print ("Material name = " .. line1.Material)

  line1.Material = "SettedMaterial"
  line1.Alpha0   = 0.8

  print ("Alpha0 value = " .. tostring (line1.Alpha0))
  print ("Material name = " .. line1.Material)
   
  line1.Color1    = vec4 (1, 0, 0, 0.7)

  print ("Color1 = " .. tostring (line1.Color1))

  line1:SetColor (0, 0, 0, 1, 0.3)

  print ("Color0 = " .. tostring (line1.Color0))

  line1:SetColor (1, 0, 1, 0)

  print ("Color1 = " .. tostring (line1.Color1))
  
end

function test_static_mesh ()
  print ("StaticMesh test")

  local visual_model1 = Scene.StaticMesh.Create ()

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

function print_text_line_colors (text_line)
  print ("Colors: ")
  
  for i = 0, #text_line.Text - 1 do
    print ("  " .. tostring (text_line:CharColor (i))) 
  end

  print ("Color factors: ")
  
  for i = 0, #text_line.Text - 1 do
    print ("  " .. tostring (text_line:CharColorFactor (i))) 
  end
end

function test_text_line ()
  print ("TextLine test")

  local font_library = Media.FontLibrary.Create ()

  local text_line1 = Scene.TextLine.Create (font_library)

  print ("Text = " .. text_line1.Text)
  print ("Font = " .. text_line1.Font)
  print ("Color = " .. tostring (text_line1.Color))
  print ("Horizontal aligment = " .. get_name (text_line1.HorizontalAlignment) .. " vertical aligment = " .. get_name (text_line1.VerticalAlignment))
  print ("Spacing Multiplier = " .. text_line1.SpacingMultiplier)

  local font_params = Media.FontCreationParams.Create ()

  font_params.FontSize = 50

  text_line1.Text                = "text"
  text_line1.Font                = "font"
  text_line1.FontCreationParams  = font_params
  text_line1.Color               = vec4 (0.1, 0.2, 0.3, 0.4)
  text_line1.HorizontalAlignment = Scene.TextLineAlignment.Center
  text_line1.VerticalAlignment   = Scene.TextLineAlignment.Bottom
  text_line1.SpacingMultiplier   = 2

  print ("Text = " .. text_line1.Text)
  print ("Font = " .. text_line1.Font)
  print ("Color = " .. tostring (text_line1.Color))
  print ("Horizontal aligment = " .. get_name (text_line1.HorizontalAlignment) .. " vertical aligment = " .. get_name (text_line1.VerticalAlignment))
  print ("Spacing Multiplier = " .. text_line1.SpacingMultiplier)

  text_line1.Text = "Non-unicode text"

  print ("Text = " .. text_line1.Text)

  text_line1:SetColor (0.4, 0.3, 0.2, 0.1)

  print ("Color = " .. tostring (text_line1.Color))

  text_line1:SetColor (0.1, 0.2, 0.3)

  print ("Color = " .. tostring (text_line1.Color))

  text_line1:SetAlignment (Scene.TextLineAlignment.Right, Scene.TextLineAlignment.Center)

  print ("Horizontal aligment = " .. get_name (text_line1.HorizontalAlignment) .. " vertical aligment = " .. get_name (text_line1.VerticalAlignment))

  text_line1:SetAlignment (Scene.TextLineAlignment.BaseLine, Scene.TextLineAlignment.BaseLine)

  print ("Horizontal aligment = " .. get_name (text_line1.HorizontalAlignment) .. " vertical aligment = " .. get_name (text_line1.VerticalAlignment))
  
  text_line1:SetCharsColorFactors (1, 2, vec4 (0, 1, 0.5, 1))
  
  print_text_line_colors (text_line1)
end

local function get_mode_name (mode)
  if mode == Scene.PageCurlMode.SinglePage then 
    return "SinglePage"
  elseif mode == Scene.PageCurlMode.DoublePageSingleMaterial then
    return "DoublePageSingleMaterial"
  elseif mode == Scene.PageCurlMode.DoublePageDoubleMaterial then
    return "DoublePageDoubleMaterial"
  else
    return "Unknown"
  end  
end

local function print_curl (curl)
  print (string.format ("  mode                              %s",                     get_mode_name (curl.Mode)))
  print (string.format ("  front left material               '%s'",                   curl:PageMaterial (Scene.PageCurlPageType.FrontLeft)))
  print (string.format ("  front right material              '%s'",                   curl:PageMaterial (Scene.PageCurlPageType.FrontRight)))
  print (string.format ("  back left material                '%s'",                   curl:PageMaterial (Scene.PageCurlPageType.BackLeft)))
  print (string.format ("  back right material               '%s'",                   curl:PageMaterial (Scene.PageCurlPageType.BackRight)))
  print (string.format ("  size                              %.2fx%.2f",              curl.Size.x, curl.Size.y))
  print (string.format ("  curl point                        %.2f; %.2f",             curl.CurlPoint.x, curl.CurlPoint.y))
  print (string.format ("  curl point position               %.2f; %.2f",             curl.CurlPointPosition.x, curl.CurlPointPosition.y))
  print (string.format ("  curl radius                       %.2f",                   curl.CurlRadius))
  print (string.format ("  minimum curl radius               %.2f",                   curl.MinimumCurlRadius))
  print (string.format ("  grid size                         %ux%u",                  curl.GridSize.x, curl.GridSize.y))
  print (string.format ("  page color                        %.3f; %.3f; %.3f; %.3f", curl.PageColor.x, curl.PageColor.y, curl.PageColor.z, curl.PageColor.w))
  print (string.format ("  corner shadow offset              %.2f",                   curl.CornerShadowOffset))
  print (string.format ("  shadow width                      %.2f",                   curl.ShadowWidth))
  print (string.format ("  shadow log base                   %.2f",                   curl.ShadowLogBase))
  print (string.format ("  shadow min log value              %.2f",                   curl.ShadowMinLogValue))
  print (string.format ("  find best curl steps              %u",                     curl.FindBestCurlSteps))
  print (string.format ("  binding mismatch weight           %.2f",                   curl.BindingMismatchWeight))
end

local function test_page_curl ()
  print ("PageCurl test")

  local curl = Scene.PageCurl.Create ()

  print ("default curl:")
  print_curl (curl)

  curl:SetPageMaterial      (Scene.PageCurlPageType.Front,     "front_material")
  curl:SetPageMaterial      (Scene.PageCurlPageType.BackLeft,  "back_left_material")
  curl:SetPageMaterial      (Scene.PageCurlPageType.BackRight, "back_right_material")
  curl:SetSize              (2, 4)
  curl:SetCurlPointPosition (1, 2)
  curl:SetGridSize          (10, 20)
  curl:SetPageColor         (0.5, 0.25, 0.125, 0.25)

  curl.Mode                          = Scene.PageCurlMode.DoublePageDoubleMaterial
  curl.CurlPoint                     = Scene.PageCurlCorner.RightBottom
  curl.CurlRadius                    = 4
  curl.MinimumCurlRadius             = 2
  curl.CornerShadowOffset            = 16
  curl.ShadowWidth                   = 8
  curl.ShadowLogBase                 = 2
  curl.ShadowMinLogValue             = 0
  curl.FindBestCurlSteps             = 500
  curl.BindingMismatchWeight         = 2

  print ("changed curl:")
  print_curl (curl)
  
  curl.Size              = vec2 (4, 2)
  curl.CurlPointPosition = vec2 (2, 1)
  curl.PageColor         = vec4 (0.25, 0.125, 0.25, 0.5)
  curl:SetCurlPoint (vec2 (0.25, 0.5))

  print ("second changed curl:")
  print_curl (curl)
end

function test_move_to_node_point_controller ()
  print ("MoveToNodePoint controller test")

  local scene = Scene.Scene.Create ()
  local node1 = Scene.Node.Create ()
  local node2 = Scene.Node.Create ()
  
  node1:BindToScene (scene)
  node2:BindToParent (node1)
  
  local mover1 = Scene.Controllers.MoveToNodePoint.Create (node1)
  local mover2 = Scene.Controllers.MoveToNodePoint.Create (node2)

  local evaluator = Scene.AccelerationEvaluators.Linear.Create ()
  
  evaluator.Acceleration = 1
  evaluator.Deceleration = 1
  evaluator.MaxSpeed     = 0.5
  
  mover1.AccelerationHandler = evaluator
  mover2.AccelerationHandler = evaluator
  
  mover1:Start (scene.Root, vec3 (10, 0, 0))
  mover2:Start (node1, vec3 (0, 10, 0))
  
  scene.Root:Update (0)  
  scene.Root:Update (1)

  print (string.format ("node1 position is %f %f %f, node2 position is %f %f %f", node1.WorldPosition.x, node1.WorldPosition.y, node1.WorldPosition.z, node2.WorldPosition.x, node2.WorldPosition.y, node2.WorldPosition.z))
  
  mover1:Stop ()
  mover2:Stop ()
  
  scene.Root:Update (2)

  print (string.format ("node1 position is %f %f %f, node2 position is %f %f %f", node1.WorldPosition.x, node1.WorldPosition.y, node1.WorldPosition.z, node2.WorldPosition.x, node2.WorldPosition.y, node2.WorldPosition.z))
end

function test_look_to_node_point_controller ()
  print ("LookToNodePoint controller test")

  local scene = Scene.Scene.Create ()
  local node1 = Scene.Node.Create ()
  
  node1:BindToScene (scene)
  
  local mover1 = Scene.Controllers.LookToNodePoint.Create (node1)

  local evaluator = Scene.AccelerationEvaluators.Linear.Create ()
  
  evaluator.Acceleration = 10
  evaluator.Deceleration = 10
  evaluator.MaxSpeed     = 5
  
  mover1.AccelerationHandler = evaluator
  
  mover1:Start (scene.Root, vec3 (10, 0, 0), Scene.NodeOrt.Z, Scene.NodeOrt.Y)
  
  scene.Root:Update (0)  
  scene.Root:Update (0.2)

  print (string.format ("node1 orientation is %f %f %f %f", node1.WorldOrientation.x, node1.WorldOrientation.y, node1.WorldOrientation.z, node1.WorldOrientation.w))
  
  mover1:Stop ()
  
  scene.Root:Update (0.3)

  print (string.format ("node1 orientation is %f %f %f %f", node1.WorldOrientation.x, node1.WorldOrientation.y, node1.WorldOrientation.z, node1.WorldOrientation.w))
end

function test_align_with_node_controller ()
  print ("AlignWithNode controller test")

  local scene = Scene.Scene.Create ()
  local node1 = Scene.Node.Create ()
  local node2 = Scene.Node.Create ()
  
  node1:BindToScene (scene)
  node2:BindToScene (scene)
  
  local mover1 = Scene.Controllers.AlignWithNode.Create (node2)

  local evaluator = Scene.AccelerationEvaluators.Linear.Create ()
  
  evaluator.Acceleration = 10
  evaluator.Deceleration = 10
  evaluator.MaxSpeed     = 5
  
  mover1.AccelerationHandler = evaluator
  
  mover1:Start (node1, Scene.NodeOrt.Z, Scene.NodeOrt.Z, Scene.NodeOrt.X)
  
  node1:SetEulerOrientation (90, 0, 0, Scene.NodeTransformSpace.World)
  
  scene.Root:Update (0)  
  scene.Root:Update (0.2)

  print (string.format ("node2 orientation is %f %f %f %f", node2.WorldOrientation.x, node2.WorldOrientation.y, node2.WorldOrientation.z, node2.WorldOrientation.w))
  
  mover1:Stop ()
  
  scene.Root:Update (0.3)

  print (string.format ("node2 orientation is %f %f %f %f", node2.WorldOrientation.x, node2.WorldOrientation.y, node2.WorldOrientation.z, node2.WorldOrientation.w))
end

local function test_scene_input ()
  print ("Scene input test")

  local scene  = Scene.Scene.Create ()
  local camera = Scene.OrthoCamera.Create ()

  camera.Left = 0
  camera.Right = 100
  camera.Top = 0
  camera.Bottom = -100
  camera.ZNear = -1
  camera.ZFar = 1
  camera.Name = "camera1"
    
  camera:Translate (0, 0, 1)
  
  camera:BindToScene (scene)
    
  local zone = Scene.InputZone.Create ()
    
  zone.Name = "zone1"
    
  zone:Translate (50, -50, 0)
  zone.Scale = vec3 (10, 10, 1)
  
  local function InputHandler (viewport, notification_id, context)
    print (string.format ("viewport='%s' notification='%s' touch=%d button=%d world_position=%s local_position=%s",
      viewport.Name, Scene.InputZone.GetNotificationName (notification_id), context.TouchId, context.Button, tostring (context.TouchWorldPosition), tostring (context.TouchLocalPosition)))
  end
    
  zone:RegisterNotificationHandler (Scene.InputZone.CreateNotificationHandler (InputHandler))

  zone:BindToScene (scene)    
    
  local screen = Scene.Screen.Create ()
  local viewport = Scene.Viewport.Create ()

  viewport.Name = "viewport1"
  viewport.Camera = camera
  viewport:SetArea (100, 100, 800, 800)    
  screen:SetArea (0, 0, 1000, 1000)
    
  screen:Attach (viewport)

  local cam = viewport.Camera

  print ("check camera name " .. cam.Name)
  
  local manager = Scene.InputManager.Create ()

  manager.Screen = screen
  manager:SetTouchSize (2, Scene.InputTransformSpace.Camera)

  manager:ProcessEvent ("CursorX axis 0")
  manager:ProcessEvent ("CursorY axis 0")
  manager:ProcessEvent ("Mouse0 down")
  manager:ProcessEvent ("Mouse0 up")
end

function test ()
  test_node ()    
  
  test_node_pivot ()
  
  test_events ()
  
  test_node_properties ()
  
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

  test_static_mesh ()

  test_sprite ()
  test_line ()

  test_scene ()

  test_text_line ()

  test_page_curl ()

  test_look_to_node_point_controller ()
  test_move_to_node_point_controller ()
  test_align_with_node_controller ()  
  
  test_scene_input ()
end
