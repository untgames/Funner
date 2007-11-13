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
  print ("funner.math.vec(7): " .. tostring (funner.math.vec (7)))
end
