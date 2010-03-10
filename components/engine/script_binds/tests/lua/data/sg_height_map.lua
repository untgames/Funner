function dump_map (height_map)
  local rows_count    = height_map.RowsCount
  local columns_count = height_map.ColumnsCount

  print ("height map " .. rows_count .. "x" .. columns_count .. ":")
  
  for i = 0, rows_count - 1 do
    print ("row " .. i .. ":")
    
    for j = 0, columns_count - 1 do
      print ("cell " .. i .. "," .. j .. ": h=" .. string.format ("%1.1f", height_map:GetVertexHeight (i, j)) .. 
      " n=[" .. tostring (height_map:GetVertexNormal (i, j)) .. "]" .. " c=[" .. tostring (height_map:GetVertexColor (i, j)) .. "]")
    end
  end
end

function test ()
  print ("HeightMap test")

  local height_map1 = Scene.HeightMap.Create ()

  dump_map (height_map1)

  height_map1:SetCellsCount (2, 3)

  dump_map (height_map1)
  
  height_map1:SetVerticesHeight (0.8)
  height_map1:SetVerticesNormal (vec3 (0.1, 0.2, 0.3))
  height_map1:SetVerticesColor (vec4 (0.4, 0.3, 0.2, 0.1))
  height_map1:SetVertexHeight (1, 2, 1.2)
  height_map1:SetVertexNormal (1, 1, vec3 (0.3, 0.2, 0.1))
  height_map1:SetVertexColor  (0, 1, vec4 (0.1, 0.2, 0.3, 0.4))
  
  dump_map (height_map1)
  
  height_map1.RowsCount = 3
  height_map1.ColumnsCount = 2

  dump_map (height_map1)
end
