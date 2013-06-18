function test ()
  local node = Scene.Node.Create ()
  local pos  = vec3 (1, 2, 3)
  local count = 1000000

  for i = 1, count do
    node.Position = pos
  end

  return count
end
