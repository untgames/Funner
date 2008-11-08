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
end
