function test (v)
  print ("Input:     " .. tostring (v))
  print ("Get v.x:   " .. tostring (v:x ()))
  print ("Get v.y:   " .. v:y ())
  print ("Get v.z:   " .. v:z ())
  print ("Get v.xzy: " .. tostring (v:xzy ()))

  tmp = v:xyz () + v:yzx ()

  print ("tmp = v:xyz + v:yzx: " .. tostring (tmp))

  tmp = tmp - v * v:zxy ()

  print ("tmp = tmp - v * v:zxy (): " .. tostring (tmp))
  print ("funner.math.vec3(4,5,6): " .. tostring (funner.math.vec3 (4, 5, 6)))

  v1 = funner.math.vec (7)

  print ("v1 = funner.math.vec(7): " .. tostring (v1))
  print ("funner.math.normalize(v1): " .. tostring (funner.math.normalize(v1)))

  v1 = funner.math.abs (funner.math.vec3 (-1, 2, -3))

  print ("v1 = funner.math.abs (funner.math.vec3 (-1, 2, -3)): " .. tostring (v1))

  v2 = funner.math.min (v1, tmp)

  print ("v2 = funner.math.min (v1, tmp): " .. tostring (v2))

  v2 = funner.math.max (v1, tmp)

  print ("v2 = funner.math.max (v1, tmp): " .. tostring (v2))

  v3 = funner.math.cross (v1, v2)

  print ("v3 = funner.math.cross (v1, v2): " .. tostring (v3))
  print ("funner.math.qlength (v3): " .. funner.math.qlength (v3))
end

function test_matrix (m)
  print ("Input:   " .. tostring (m:row (0)))
--  print ("         " .. tostring (m:row (1)))
--  print ("         " .. tostring (m:row (2)))
--  print ("row2:    " .. tostring (m:row (2)))
--  print ("column1: " .. tostring (m:column (1)))

  m1 = funner.math.mat4 (2)
  m2 = funner.math.mat4 (3)
  m3 = m1 * m2
  print ("         " .. tostring (m3:row (0)))
 
  v1 = funner.math.vec (7)
  v2 = funner.math.multiply_mat_vec (m1, v1)
  print ("         " .. tostring (v2))
  m5 = funner.math.mat4 (2)
  print ("minor m 1 1 = " .. m5:minor (1, 1))
  print ("det m = " .. m5:det ())
  m = m:set_element (0, 1, 15.4)
  print ("         " .. tostring (m:row (0)))
end