function ConvertStringToVec3(str)
  assert(str,"ERROR: ConvertStringToVec3(str) - str is nil")
  assert(type(str)=="string","ERROR: ConvertStringToVec3(str) - str is not a string")
  local x,y,z=ToNumbers( UnpackString( str, " "))
  assert((x and y),"ERROR: ConvertToVec3("..str..") - 'x' or 'y' is nil")
  --возможно что были заданы только координаты x и y - тогда z=0
  local v=0
  if z then
    v=vec3(x,y,z)
  else
    v=vec3(x,y,0)
  end
  return v
end
