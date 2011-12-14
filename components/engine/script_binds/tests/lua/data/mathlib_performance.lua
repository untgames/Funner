function test ()
  local v = vec3 (0)

  for i = 1, 1000000 do
    v = v + vec3 (i):normalize ()
  end

  return v.length
end
