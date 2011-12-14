function test ()
  print ("mathlib spline loader test")
  
  local node = Common.StringNode.LoadXml ("data/splines.xml")
  
  local splines_count = node.ChildrenCount
    
  for i = 0, splines_count - 1 do
    local spline_node = node:Child (i)
    
    if spline_node.Name ==  "Spline" then
      local spline = nil
    
      local spline_type = spline_node:Get ("Type")
      
      if spline_type == "float" then
        spline = Math.SplineLoader.LoadLinearFloatSpline (spline_node, "Key", "Time", "Value")      
      elseif spline_type == "vec2f" then
        spline = Math.SplineLoader.LoadLinearVec2fSpline (spline_node, "Key", "Time", "Value")
      elseif spline_type == "vec3f" then
        spline = Math.SplineLoader.LoadLinearVec3fSpline (spline_node, "Key", "Time", "Value", " ;")
      elseif spline_type == "vec4f" then
        spline = Math.SplineLoader.LoadLinearVec4fSpline (spline_node, "Key", "Time", "Value", " ;")
      end
      
      print ("Loaded spline with type '" .. spline_type .. "'")
      print ("Keys count = " .. spline.KeysCount)
    end
  end
end
