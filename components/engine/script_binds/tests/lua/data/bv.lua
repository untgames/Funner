function test ()
  print ("Axis aligned box test")

  local aabox1 = BoundVolumes.AxisAlignedBox.Create ()

  print ("minimum = " .. tostring (aabox1.Minimum))
  print ("maximum = " .. tostring (aabox1.Maximum))
  print ("center = " .. tostring (aabox1.Center))
  print ("size = " .. tostring (aabox1.Size))
  print ("radius = " .. tostring (aabox1.Radius))
  print ("empty = " .. tostring (aabox1.Empty))

  aabox1.Minimum = vec3 (-10, -24, 7.45)
  aabox1.Maximum = vec3 (17, 32, 14.59)

  print ("pxnynz corner = " .. tostring (aabox1:Corner (BoundVolumes.Corner.pxnynz)))

  print ("minimum = " .. tostring (aabox1.Minimum))
  print ("maximum = " .. tostring (aabox1.Maximum))
  print ("center = " .. tostring (aabox1.Center))
  print ("size = " .. tostring (aabox1.Size))
  print ("radius = " .. tostring (aabox1.Radius))
  print ("empty = " .. tostring (aabox1.Empty))

  local aabox2 = AABB (vec3 (-5, -4, -3), vec3 (4, 5, 6))

  print ("box2 minimum = " .. tostring (aabox2.Minimum))
  print ("box2 maximum = " .. tostring (aabox2.Maximum))

  local aabox3 = aabox1 + aabox2

  print ("box3 minimum = " .. tostring (aabox3.Minimum))
  print ("box3 maximum = " .. tostring (aabox3.Maximum))

  aabox3 = aabox1 * mat4 (2)

  print ("box3 minimum = " .. tostring (aabox3.Minimum))
  print ("box3 maximum = " .. tostring (aabox3.Maximum))

  aabox1:Reset (vec3 (1, 1, 1))

  print ("box3 volume = " .. tostring (aabox3.Volume))
  print ("box1 volume = " .. tostring (aabox1.Volume))

  print ("is box2 intersects box3 ? " .. tostring (BoundVolumes.AxisAlignedBox.Intersects (aabox2, aabox3)))
  print ("is box2 contatins box3 ? " .. tostring (BoundVolumes.AxisAlignedBox.Contains (aabox2, aabox3)))
  aabox2:SetExtents (vec3 (-10, -10, -10), vec3 (5, 5, 5))
  aabox3:SetExtents (vec3 (-5, -5, -5), vec3 (4, 4, 4))
  print ("is transformed box2 intersects box3 ? " .. tostring (BoundVolumes.AxisAlignedBox.Intersects (aabox2, aabox3)))
  print ("is transformed box2 contatins box3 ? " .. tostring (BoundVolumes.AxisAlignedBox.Contains (aabox2, aabox3)))

  aabox3:SetExtents (vec3 (-6, -6, -6), vec3 (14, 14, 14))

  aabox4 = aabox2:Intersection (aabox3)
  
  print ("Intersection minimum = " .. tostring (aabox4.Minimum))
  print ("Intersection maximum = " .. tostring (aabox4.Maximum))

--  print ("box3 == box1 : " .. tostring (aabox1 == aabox3))
--  print ("box1 == box1 : " .. tostring (aabox1 == aabox1))

  print ("minimum = " .. tostring (aabox1.Minimum))
  print ("maximum = " .. tostring (aabox1.Maximum))
  print ("center = " .. tostring (aabox1.Center))
  print ("size = " .. tostring (aabox1.Size))
  print ("radius = " .. tostring (aabox1.Radius))
  print ("empty = " .. tostring (aabox1.Empty))
end
