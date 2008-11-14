function test ()
  print ("Axis aligned box test")

  aabox1 = BV.AxisAlignedBox.create ()
  aabox2 = BV.AxisAlignedBox.create ()

  print ("minimum = " .. tostring (aabox1.minimum))
  print ("maximum = " .. tostring (aabox1.maximum))
  print ("center = " .. tostring (aabox1.center))
  print ("size = " .. tostring (aabox1.size))
  print ("radius = " .. tostring (aabox1.radius))
  print ("empty = " .. tostring (aabox1.empty))

  aabox1.minimum = vec3 (-10, -24, 7.45)
  aabox1.maximum = vec3 (17, 32, 14.59)

  print ("pxnynz corner = " .. tostring (aabox1:get_corner (BV.Corner.pxnynz)))

  print ("minimum = " .. tostring (aabox1.minimum))
  print ("maximum = " .. tostring (aabox1.maximum))
  print ("center = " .. tostring (aabox1.center))
  print ("size = " .. tostring (aabox1.size))
  print ("radius = " .. tostring (aabox1.radius))
  print ("empty = " .. tostring (aabox1.empty))

  aabox2:set_extents (vec3 (-5, -4, -3), vec3 (4, 5, 6))

  print ("box2 minimum = " .. tostring (aabox2.minimum))
  print ("box2 maximum = " .. tostring (aabox2.maximum))

  local aabox3 = aabox1 + aabox2

  print ("box3 minimum = " .. tostring (aabox3.minimum))
  print ("box3 maximum = " .. tostring (aabox3.maximum))

  aabox3 = aabox1 * mat4 (2)

  print ("box3 minimum = " .. tostring (aabox3.minimum))
  print ("box3 maximum = " .. tostring (aabox3.maximum))

  aabox1:reset (vec3 (1, 1, 1))

  print ("box3 volume = " .. BV.AxisAlignedBox.volume (aabox3) .. " = " .. aabox3:volume ())
  print ("box1 volume = " .. aabox1:volume ())

  print ("is box2 intersects box3 ? " .. tostring (BV.AxisAlignedBox.intersects (aabox2, aabox3)))
  print ("is box2 contatins box3 ? " .. tostring (BV.AxisAlignedBox.contains (aabox2, aabox3)))
  aabox2:set_extents (vec3 (-10, -10, -10), vec3 (5, 5, 5))
  aabox3:set_extents (vec3 (-5, -5, -5), vec3 (4, 4, 4))
  print ("is transformed box2 intersects box3 ? " .. tostring (BV.AxisAlignedBox.intersects (aabox2, aabox3)))
  print ("is transformed box2 contatins box3 ? " .. tostring (BV.AxisAlignedBox.contains (aabox2, aabox3)))

  aabox3:set_extents (vec3 (-6, -6, -6), vec3 (14, 14, 14))

  aabox4 = BV.AxisAlignedBox.intersection (aabox2, aabox3)
  
  print ("Intersection minimum = " .. tostring (aabox4.minimum))
  print ("Intersection maximum = " .. tostring (aabox4.maximum))

--  print ("box3 == box1 : " .. tostring (aabox1 == aabox3))
--  print ("box1 == box1 : " .. tostring (aabox1 == aabox1))

  print ("minimum = " .. tostring (aabox1.minimum))
  print ("maximum = " .. tostring (aabox1.maximum))
  print ("center = " .. tostring (aabox1.center))
  print ("size = " .. tostring (aabox1.size))
  print ("radius = " .. tostring (aabox1.radius))
  print ("empty = " .. tostring (aabox1.empty))
end
