function test (v)
  print ("Input:     " .. tostring (v))
  print ("Get v.x:   " .. tostring (v.x))
  print ("Get v.y:   " .. v.y)
  print ("Get v.z:   " .. v.z)

  print ("Get v.xzy: " .. tostring (v.xzy))

  tmp = v.xyz + v.yzx

  print ("tmp = v.xyz + v.yzx: " .. tostring (tmp))

  tmp = tmp - v * v.zxy

  print ("tmp = tmp - v * v.zxy: " .. tostring (tmp))

  print ("vec3(4,5,6): " .. tostring (vec3 (4, 5, 6)))

  v1 = scalar3 (7)

  print ("v1 = scalar3(7): " .. tostring (v1))
  print ("v1:normalize(): " .. tostring (v1:normalize()))

  v1 = vec3 (-1, 2, -3):abs ()

  print ("v1 = vec3 (-1, 2, -3):abs (): " .. tostring (v1))

  v2 = v1:min (tmp)

  print ("v2 = v1:min (tmp): " .. tostring (v2))

  v2 = v1:max (tmp)

  print ("v2 = v1:max (tmp): " .. tostring (v2))

  v3 = v1:cross (v2)

  print ("v3 = v1:cross (v2): " .. tostring (v3))
  print ("v3.qlength: " .. v3.qlength)

  v1 = v.xyz -- ����� �����������, � �� �������� ������ �� ������

  v1 [0] = 4
  v1 [1] = 5
  v1.z   = 6

  print ("After set values: " .. tostring (v1))

end

function test_matrix (m)
  print ("Input:   ");

  print (m [0])
  print (m [1])
  print (m [2])
  print (m [3])

  m [0].zxy = vec3 (1, 2, 3)

  print ("After m[0].zxy=vec3(1,2,3): " .. tostring (m [0]))

  local m1 = mat4 (2)
  local m2 = mat4 (3)
  local m3 = m1 * m2

  print ("Result of matrix multiplies: " .. tostring (m3))

  m3:set (1, 1, 123)

  print ("After m3:set(1,1,123): " .. tostring (m3))

  m3 [2][2] = 456

  print ("After m3 [2][2]=456: " .. tostring (m3))

  local m5 = mat4 (2)

  print ("mat4(2):det (): " .. m5:det ()) 
end
