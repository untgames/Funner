local function CreateClass ()
  local private_var = 0

  local methods = {}
  
  methods.SetPrivateVar = function (x) private_var = x end
  methods.GetPrivateVar = function () return private_var end

  return methods
end

local object = CreateClass ()

object.SetPrivateVar (1)

print (string.format ("var value=%d", object.GetPrivateVar ()))
