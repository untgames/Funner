function create_float_value (v)
  return v
end

function create_vec2_value (v)
  return vec2 (v)
end

function create_vec3_value (v)
  return vec3 (v)
end

function create_vec4_value (v)
  return vec4 (v)
end

function wrap_name (wrap)
  if     wrap == Math.SplineWrap.Clamp  then return "Clamp"
  elseif wrap == Math.SplineWrap.Mirror then return "Mirror"
  elseif wrap == Math.SplineWrap.Repeat then return "Repeat"
  else                                       return "Unknown"  
  end    
end

function test_spline (spline, create_value, spline_name)
  print ("Testing spline " .. spline_name .. ":")

  print ("Spline default begin wrap is " .. wrap_name (spline.BeginWrap))
  print ("Spline default end wrap is " .. wrap_name (spline.EndWrap))
  print ("Spline keys count is " .. spline.KeysCount)
  print ("Spline empty is " .. tostring (spline.Empty))
  print ("Spline capacity is " .. spline.Capacity)
  print ("Spline closed is " .. tostring (spline.Closed))
  print ("Spline closed_eps is " .. spline.ClosedEps)
  print ("Spline min_time is " .. spline.MinTime)
  print ("Spline max_time is " .. spline.MaxTime)
  
  spline:Reserve (10)
  spline:Wrap (Math.SplineWrap.Repeat, Math.SplineWrap.Mirror)
  spline:AddKey (1, create_value (0))
  spline:AddKey (2, create_value (2))
  spline:Sort ()

  print ("Spline default begin wrap is " .. wrap_name (spline.BeginWrap))
  print ("Spline default end wrap is " .. wrap_name (spline.EndWrap))
  print ("Spline keys count is " .. spline.KeysCount)
  print ("Spline empty is " .. tostring (spline.Empty))
  print ("Spline capacity is " .. spline.Capacity)
  print ("Spline closed is " .. tostring (spline.Closed))
  print ("Spline closed_eps is " .. spline.ClosedEps)
  print ("Spline min_time is " .. spline.MinTime)
  print ("Spline max_time is " .. spline.MaxTime)
  
  print ("key 0 time is " .. spline:GetKeyTime (0))
  print ("key 0 value is '" .. tostring (spline:GetKeyValue (0)) .. "'")
  print ("key 1 time is " .. spline:GetKeyTime (1))
  print ("key 1 value is '" .. tostring (spline:GetKeyValue (1)) .. "'")
  
  spline:SetKey (1, 3, create_value (3))
  
  print ("key 1 time is " .. spline:GetKeyTime (1))
  print ("key 1 value is '" .. tostring (spline:GetKeyValue (1)) .. "'")
  
  for i = -1, 4 do
    print ("spline value for time " .. i .. " is '" .. tostring (spline:Eval (i)) .."'")
  end
  
  spline:Wrap (Math.SplineWrap.Clamp, Math.SplineWrap.Clamp)
  
  for i = -1, 4 do
    print ("spline value for time " .. i .. " is '" .. tostring (spline:Eval (i)) .."'")
  end

  spline:RemoveKey (0)  

  print ("Spline keys count is " .. spline.KeysCount)
  
  spline:Clear ()
  
  print ("Spline keys count is " .. spline.KeysCount)
end

function test ()
  test_spline (Math.LinearSplineFloat.Create (), create_float_value, "Math.LinearSplineFloat")
  test_spline (Math.LinearSplineVec2.Create (), create_vec2_value, "Math.LinearSplineVec2")
  test_spline (Math.LinearSplineVec3.Create (), create_vec3_value, "Math.LinearSplineVec3")
  test_spline (Math.LinearSplineVec4.Create (), create_vec4_value, "Math.LinearSplineVec4")
end
